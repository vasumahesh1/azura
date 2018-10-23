import Common;

struct RWBlock {
    RWTexture2D<float4> rwTexture;
};

ParameterBlock<LightUBO> lightBlock;
ParameterBlock<RWBlock> rwBlock;

const static int lightsPerThread = NUM_LIGHTS / 32;

[numthreads(32, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    int offset = lightsPerThread * DTid.x;

    for(int idx = offset; idx < offset + lightsPerThread; ++idx) {
        Light currentLight = lightBlock.lights[idx];
        float2 upper = float2(idx, 0);
        float2 lower = float2(idx, 1);

        rwBlock.rwTexture[upper] = float4(currentLight.position, currentLight.radius);
        rwBlock.rwTexture[lower] = float4(currentLight.color, 1.0);
    }
}