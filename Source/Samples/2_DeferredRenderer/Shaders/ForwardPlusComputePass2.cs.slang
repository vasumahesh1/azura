import Common;
import ForwardPlusComputeCommonPass2;

groupshared uint minDepthInt;
groupshared uint maxDepthInt;

groupshared uint groupLightCount;
groupshared uint groupLightIndices[MAX_LIGHTS_PER_CLUSTER];

[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void main(
    uint3 groupID : SV_GroupID,
    uint3 groupThreadId : SV_GroupThreadID,
    uint groupIndex : SV_GroupIndex,
    uint3 dispatchThreadId : SV_DispatchThreadID
)
{
    // 1. Update Light Positions

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

    GroupMemoryBarrierWithGroupSync();


    // 2. Calculate Min and Max Depth per Cluster

    // int texCoordX = (groupID.x * WIDTH_PER_TILE) + groupThreadId.x;

    // uint2 texCoord;

    // float depthValue = attachmentBlock.depthBuffer.Load(uint3(0, 0, 0)).r;
    // uint depthInt = asuint(depthValue);

    // minDepthInt = 0xFFFFFFFF;
    // maxDepthInt = 0;

    // GroupMemoryBarrierWithGroupSync();

    // InterlockedMin(minDepthInt, depthInt);
    // InterlockedMax(maxDepthInt, depthInt);

    // GroupMemoryBarrierWithGroupSync();

    // float minGroupDepth = asfloat(minDepthInt);
    // float maxGroupDepth = asfloat(maxDepthInt);


    // 3. Light Cull

    for(uint idx = 0; idx < LIGHTS_PER_THREAD; ++idx) {
        uint lightIdx = idx * THREAD_COUNT + groupIndex;

        if (lightIdx >= NUM_LIGHTS) {
            break;
        }

        const float2 upper = float2(lightIdx, 0);
        const float2 lower = float2(lightIdx, 1);

        const float4 currentLightPosRad = rwBlock.lightTexture[upper];
        const float4 lightViewPos = mul(lightUBO.view, float4(currentLightPosRad.xyz, 1.0));

        // Check Intersection for Tile (Thread Group)
    }

    GroupMemoryBarrierWithGroupSync();

}