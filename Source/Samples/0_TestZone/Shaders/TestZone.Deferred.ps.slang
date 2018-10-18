import CommonDeferred;

float4 main(PixelShaderInput input) : SV_Target
{
  float2 flipUV = input.uv;
  flipUV.y = 1.0 - flipUV.y;
  float4 color = float4(attachmentBlock.g_attachmentTexture.Sample(samplerBlock.g_sampler, flipUV).rgb, 1.0);
  return color;
}