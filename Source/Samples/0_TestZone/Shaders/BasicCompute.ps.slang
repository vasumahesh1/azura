import Common;

ParameterBlock<UBOData> ubo;
ParameterBlock<SamplerBlock> sampState;
ParameterBlock<TextureBlock> attachment;

float4 main(UVPixelInput input) : SV_Target
{
  float4 color = float4(attachment.target.Sample(sampState.linearSampler, input.uv).rgb, 1.0);
  return color;
}