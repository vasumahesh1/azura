import Common;

struct RWBlock {
    RWTexture2D<float4> rwTexture;
};

ParameterBlock<LightUBO> lightUBO;
ParameterBlock<RWBlock> rwBlock;

const static int lightsPerThread = NUM_LIGHTS / BLOCK_SIZE_X;

[numthreads(BLOCK_SIZE_X, 1, 1)]
void main(uint3 groupID : SV_GroupID,
    uint3 groupThreadId : SV_GroupThreadID,
    uint groupIndex : SV_GroupIndex,
    uint3 dispatchThreadId : SV_DispatchThreadID)
{
    int offset = lightsPerThread * dispatchThreadId.x;

    for(int idx = offset; idx < offset + lightsPerThread; ++idx) {
        if (idx >= NUM_LIGHTS) {
            break;
        }

        float2 upper = float2(idx, 0);
        float2 lower = float2(idx, 1);

        float4 currentLightPosRad = rwBlock.rwTexture[upper];

        currentLightPosRad.y += (LIGHT_DT * lightUBO.timeDelta);

        if (currentLightPosRad.y < LIGHT_MIN.y) {
            currentLightPosRad.y = LIGHT_MAX.y;
        }

        rwBlock.rwTexture[upper] = float4(currentLightPosRad.xyz, currentLightPosRad.w);
    }
}