import Common;

struct RWBlock {
    RWTexture2D<float4> rwTexture;
};

ParameterBlock<RWBlock> rwBlock;



[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    // float2 WindowLocal = ((float2)DTid.xy / g_MaxThreadIter.xy) * float2(1, -1) + float2(-0.5f, 0.5f);
    // float2 coord = WindowLocal.xy * g_Window.xy + g_Window.zw;

    // uint maxiter = (uint)g_MaxThreadIter.z * 4;
    // uint iter = 0;
    // float2 constant = coord;
    // float2 sq;
    // do
    // {
    //     float2 newvalue;
    //     sq = coord * coord;
    //     newvalue.x = sq.x - sq.y;
    //     newvalue.y = 2 * coord.y * coord.x;
    //     coord = newvalue + constant;
    //     iter++;
    // } while (iter < maxiter && (sq.x + sq.y) < 4.0);

    // float colorIndex = frac((float)iter / g_MaxThreadIter.z);
    // float4 SampledColor = ColorMapTexture.SampleLevel(ColorMapSampler, float2(colorIndex, 0), 0);

    float4 finalColor;
    if (DTid.x < 16) {
        finalColor = float4(1, 1, 1, 1);
    }
    else {
        finalColor = float4(0, 0, 0, 1);
    }

    rwBlock.rwTexture[DTid.xy] = finalColor;
}