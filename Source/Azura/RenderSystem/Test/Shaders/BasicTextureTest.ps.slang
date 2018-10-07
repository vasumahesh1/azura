import Common;

[[vk::binding(0,1)]] SamplerState     linearSampler : register(s1);
[[vk::binding(1,1)]] Texture2D        targetTexture : register(t1);

float4 main(UVPixelInput input) : SV_Target
{
  float4 color = float4(targetTexture.Sample(linearSampler, input.uv).rgb, 1.0);
  return color;
}