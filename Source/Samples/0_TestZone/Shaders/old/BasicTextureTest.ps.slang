import Common;

ParameterBlock<UBOData> ubo;
ParameterBlock<TextureState> textureState;

float4 main(UVPixelInput input) : SV_Target
{
  float4 color = float4(textureState.target.Sample(textureState.linearSampler, input.uv).rgb, 1.0);
  return color;
}