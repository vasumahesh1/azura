import Common;

[[vk::binding(0,1)]] Texture2D        g_txDiffuse : register(t1);
[[vk::binding(1,1)]] SamplerState     g_sampler : register(s1);

float4 main(PixelShaderInput input) : SV_Target
{
  float4 color = float4(g_txDiffuse.Sample(g_sampler, input.uv).rgb, 1.0);
  return color;
}