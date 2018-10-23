import CommonDeferredPost;

float4 main(PixelShaderInput input) : SV_Target
{
  float4 color = float4(attachmentBlock.g_attachmentTexture.Sample(samplerBlock.g_sampler, input.uv).rgb, 1.0);
  return color;
}