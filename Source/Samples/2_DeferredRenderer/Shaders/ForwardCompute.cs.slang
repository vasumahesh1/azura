import Common;

struct RWBlock {
    RWTexture2D<float4> rwTexture;
};

ParameterBlock<LightUBO> lightUBO;
ParameterBlock<RWBlock> rwBlock;

const static int lightsPerThread = NUM_LIGHTS / 32;

[numthreads(32, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    int offset = lightsPerThread * DTid.x;

    for(int idx = offset; idx < offset + lightsPerThread; ++idx) {
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