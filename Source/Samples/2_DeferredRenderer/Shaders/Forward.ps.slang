import Common;
import ForwardCommon;

float4 main(ColorPixelInput input) : SV_Target
{
  float4 fragColor = textureBlock.tex.Sample(samplerBlock.samp, input.uv);

  float4 normalColor = float4((input.normal + float3(1.0, 1.0, 1.0)) / 2.0, 1.0);

  return normalColor;
}
