import Common;

float4 main(PixelShaderInput input) : SV_Target
{
  float4 color = float4(samplerBlock.g_txDiffuse.Sample(samplerBlock.g_sampler, input.uv).rgb, 1.0);
  return color;
}