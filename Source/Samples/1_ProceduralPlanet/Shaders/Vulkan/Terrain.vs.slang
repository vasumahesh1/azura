import Common;

ParameterBlock<UBOData> ubo;

PixelShaderInput main(VertexShaderInput input)
{
  PixelShaderInput output;

  output.pos = mul(ubo.proj, mul(ubo.view, mul(ubo.model, input.inPosition)));
  output.color = float4(1, 1, 1, 1);
  return output;
}