import Common;
import ForwardPlusComputeCommonPass3;

float4 main(QuadInput_PS input) : SV_Target
{
  float2 screenUV = input.uv;
  screenUV.y = 1.0 - screenUV.y;

  float3 albedo = attachmentBlock.gbuffer1.Sample(samplerBlock.samp, screenUV).rgb;
  return float4(albedo, 1.0);
}
