import Common;
import ForwardPlusComputeCommonPass2;

groupshared uint minDepthInt;
groupshared uint maxDepthInt;

groupshared uint groupLightCount;
groupshared uint groupLightIndices[MAX_LIGHTS_PER_CLUSTER];


struct Plane
{
    float3 N;
    float  d;
};

Plane computePlane(float3 p0, float3 p1, float3 p2)
{
    Plane plane;
 
    float3 v0 = p1 - p0;
    float3 v2 = p2 - p0;
 
    plane.N = normalize( cross( v0, v2 ) );
 
    // Compute the distance to the origin using p0.
    plane.d = dot(plane.N, p0);
 
    return plane;
}

struct Frustum
{
    Plane planes[4];
};


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


    // uint screenX1 = groupID.x * WIDTH_PER_TILE;
    // uint screenY1 = groupID.y * HEIGHT_PER_TILE;

    // uint screenX2 = (groupID.x + 1) * WIDTH_PER_TILE;
    // uint screenY2 = (groupID.y + 1) * HEIGHT_PER_TILE;

    // float2 tileP1 = float2(screenX1, screenY1);
    // float2 tileP2 = float2(screenX1, screenY2);
    // float2 tileP3 = float2(screenX2, screenY2);
    // float2 tileP4 = float2(screenX2, screenY1);

    // // To NDC
    // tileP1 = tileP1 / float2(SCREEN_WIDTH, SCREEN_HEIGHT);
    // tileP2 = tileP2 / float2(SCREEN_WIDTH, SCREEN_HEIGHT);
    // tileP3 = tileP3 / float2(SCREEN_WIDTH, SCREEN_HEIGHT);
    // tileP4 = tileP4 / float2(SCREEN_WIDTH, SCREEN_HEIGHT);

    // tileP1.x = (tileP1.x * 2.0) - 1.0;
    // tileP2.x = (tileP2.x * 2.0) - 1.0;
    // tileP3.x = (tileP3.x * 2.0) - 1.0;
    // tileP4.x = (tileP4.x * 2.0) - 1.0;

    // tileP1.y = 1.0 - (2.0 * tileP1.y);
    // tileP2.y = 1.0 - (2.0 * tileP2.y);
    // tileP3.y = 1.0 - (2.0 * tileP3.y);
    // tileP4.y = 1.0 - (2.0 * tileP4.y);

    // // To View Space
    // float4 viewP1 = float4(tileP1, 1.0, 1.0) * lightUBO.farPlane;
    // float4 viewP2 = float4(tileP2, 1.0, 1.0) * lightUBO.farPlane;
    // float4 viewP3 = float4(tileP3, 1.0, 1.0) * lightUBO.farPlane;
    // float4 viewP4 = float4(tileP4, 1.0, 1.0) * lightUBO.farPlane;

    // viewP1 = lightUBO.invProj * viewP1;
    // viewP2 = lightUBO.invProj * viewP2;
    // viewP3 = lightUBO.invProj * viewP3;
    // viewP4 = lightUBO.invProj * viewP4;

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