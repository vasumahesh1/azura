import CommonDeferred;

[[vk::binding(0,1)]] SamplerState     g_sampler : register(s1);
[[vk::binding(0,2)]] Texture2D        g_txDiffuse : register(t1);

float4 main(PixelShaderInput input) : SV_Target
{
  float4 color = float4(g_txDiffuse.Sample(g_sampler, input.uv).rgb, 1.0);
  // float4 color = float4(1.0, 0.0, 0.0, 1.0);
  // float4 color = float4(input.pos.xy, 0.0, 1.0);
  return color;
}