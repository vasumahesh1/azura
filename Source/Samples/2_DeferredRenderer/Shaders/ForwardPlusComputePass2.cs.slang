import Common;
import ForwardPlusComputeCommonPass2;

#define BLOCK_SIZE 16

const static float LIGHT_DT = -2.5;
const static float3 LIGHT_MIN = {-5.0, 0.0, -5.0 };
const static float3 LIGHT_MAX = { 5.0, 15.0, 5.0 };

const static int lightsPerThread = NUM_LIGHTS / 32;

[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    int offset = lightsPerThread * DTid.x;

    for(int idx = offset; idx < offset + lightsPerThread; ++idx) {
        float2 upper = float2(idx, 0);
        // float2 lower = float2(idx, 1);

        // float4 currentLightPosRad = rwBlock.lightTexture[upper];

        // currentLightPosRad.y += (LIGHT_DT * lightUBO.timeDelta);

        // if (currentLightPosRad.y < LIGHT_MIN.y) {
        //     currentLightPosRad.y = LIGHT_MAX.y;
        // }

        rwBlock.lightTexture[upper] = float4(1.0, 1.0, 1.0, 1.0);
    }
}