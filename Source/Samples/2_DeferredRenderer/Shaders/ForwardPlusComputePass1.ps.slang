import Common;
import ForwardPlusComputeCommonPass1;

ForwardPlusComputeOut_PS main(ColorPixelInput input) : SV_Target
{
  ForwardPlusComputeOut_PS result;

  float3 albedo = textureBlock.tex.Sample(samplerBlock.samp, input.uv).rgb;
  float3 normalFromMap = textureBlock.norm.Sample(samplerBlock.samp, input.uv).rgb;
  float3 normalFromGeo = input.normal;

  float3 finalNormal = applyNormalMap(normalFromGeo, normalFromMap);

  result.target1 = float4(albedo, 1.0);
  result.target2 = float4(finalNormal, 1.0);
  result.target3 = float4(input.worldPos.xyz, 1.0);

  return result;
}
