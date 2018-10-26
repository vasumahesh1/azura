import Common;
import ForwardPlusComputeCommonPass2;

groupshared uint groupLightCount;
groupshared uint groupLightIndices[MAX_LIGHTS_PER_CLUSTER];

groupshared uint minDepthInt;
groupshared uint maxDepthInt;

#define EPSILON 0.0001

bool IsSphereBehindPlane(float3 center, float radius, Plane plane)
{
    return (dot(plane.normal, center) - plane.dist) < ((-radius) - EPSILON);
}

bool ComputeFrustumIntersection(float3 lightViewPos, float lightRadius, Frustum frustum, float minZ, float maxZ)
{
    // return true;

    bool result = true;

    // if (lightViewPos.z + lightRadius < minZ || lightViewPos.z - lightRadius > maxZ )
    // {
    //     result = false;
    // }

    for (int i = 0; i < 4 && result; i++ ) {
        if (IsSphereBehindPlane(lightViewPos, lightRadius, frustum.planes[i])) {
            result = false;
        }
    }

    return result;
}


[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void main(
    uint3 groupID : SV_GroupID,
    uint3 groupThreadId : SV_GroupThreadID,
    uint groupIndex : SV_GroupIndex,
    uint3 dispatchThreadId : SV_DispatchThreadID
)
{
    if (groupIndex == 0) {
        groupLightCount = 0;
        minDepthInt = 0xFFFFFFFF;
        maxDepthInt = 0;
    }

    GroupMemoryBarrierWithGroupSync();

    // 1. Update Light Positions

    if (groupID.x == 0 && groupID.y == 0) {
        for(uint idx = 0; idx < LIGHTS_PER_THREAD; ++idx) {
            const uint lightIdx = idx * THREAD_COUNT + groupIndex;

            if (lightIdx >= NUM_LIGHTS) {
                break;
            }

            const float2 upper = float2(lightIdx, 0);

            float4 currentLightPosRad = rwBlock.lightTexture[upper];

            currentLightPosRad.y += (LIGHT_DT * lightUBO.timeDelta);

            if (currentLightPosRad.y < LIGHT_MIN.y) {
                currentLightPosRad.y = LIGHT_MAX.y;
            }

            rwBlock.lightTexture[upper] = currentLightPosRad;
        }
    }


    // 2. Calculate Min and Max Depth per Cluster

    // float depthValue = attachmentBlock.depthBuffer.Load(uint3(dispatchThreadId.xy, 0)).r;
    // uint depthInt = asuint(depthValue);

    // GroupMemoryBarrierWithGroupSync();

    // InterlockedMin(minDepthInt, depthInt);
    // InterlockedMax(maxDepthInt, depthInt);

    // GroupMemoryBarrierWithGroupSync();

    // float minGroupDepth = asfloat(minDepthInt);
    // float maxGroupDepth = asfloat(maxDepthInt);

    // float4 minDepthVec = mul(lightUBO.invProj, float4(0, 0, minGroupDepth, 1));
    // float4 maxDepthVec = mul(lightUBO.invProj, float4(0, 0, maxGroupDepth, 1));

    // minDepthVec = minDepthVec / minDepthVec.w;
    // maxDepthVec = maxDepthVec / maxDepthVec.w;

    // minGroupDepth = minDepthVec.z;
    // maxGroupDepth = maxDepthVec.z;

    // Frustum per Thread Group
    const uint frustIdx = (groupID.x * TILES_Y) + groupID.y;
    Frustum f = frustumBlock.frustums[frustIdx];

    // 3. Light Cull
    for(uint lightIdx = groupIndex; lightIdx < NUM_LIGHTS; lightIdx += THREAD_COUNT) {
        const float2 upper = float2(lightIdx, 0);
        const float2 lower = float2(lightIdx, 1);

        const float4 currentLightPosRad = rwBlock.lightTexture[upper];
        const float4 lightViewPos = mul(lightUBO.view, float4(currentLightPosRad.xyz, 1.0));

        // Check Intersection for Tile (Thread Group)
        if (ComputeFrustumIntersection(lightViewPos.xyz, currentLightPosRad.w, f, 0.0, 0.0)) {
            uint offset;
            InterlockedAdd(groupLightCount, 1, offset);

            if (offset < MAX_LIGHTS_PER_CLUSTER) {
                groupLightIndices[offset] = lightIdx;
            }
        }
    }

    GroupMemoryBarrierWithGroupSync();

    // 4. Write to Texture
    if (groupIndex == 0) {
        const float2 countUV = float2(frustIdx, 0);
        rwBlock.clusterTexture[countUV] = float4(groupLightCount, 0.0, 0.0, 0.0);
        
        uint fullSets = floor(groupLightCount / 4) * 4;
        uint remainingSets = groupLightCount - fullSets; // can be 0 - 3 Eg 52 53 54 55 56

        uint height = 1;
        for(uint idx = 0; idx < fullSets; idx += 4) {
            const float2 writeUV = float2(frustIdx, height);
            rwBlock.clusterTexture[writeUV] = float4(groupLightIndices[idx], groupLightIndices[idx + 1], groupLightIndices[idx + 2], groupLightIndices[idx + 3]);
            ++height;
        }

        // Encode Last Set of Lights
        float4 lastEncode = float4(0.0, 0.0, 0.0, 0.0);
        float2 remainingUV = float2(frustIdx, height);
        if (remainingSets >= 1) {
            lastEncode.x = groupLightIndices[fullSets];
        }

        if (remainingSets >= 2) {
            lastEncode.y = groupLightIndices[fullSets + 1];
        }

        if (remainingSets >= 3) {
            lastEncode.z = groupLightIndices[fullSets + 2];
        }

        rwBlock.clusterTexture[remainingUV] = lastEncode;
    }
}