import Common;

ParameterBlock<UBOData> ubo;
ParameterBlock<TextureBlock> texState;
ParameterBlock<SamplerBlock> sampState;

float4 main(UVPixelInput input) : SV_Target
{
  float4 color = float4(texState.target.Sample(sampState.linearSampler, input.uv).rgb, 1.0);
  return color;
}