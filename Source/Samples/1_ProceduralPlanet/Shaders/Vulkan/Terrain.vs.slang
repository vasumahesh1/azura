import Common;

ParameterBlock<UBOData> ubo;
ParameterBlock<ShaderControls> controls;

PixelShaderInput main(VertexShaderInput input)
{
  PixelShaderInput output;

  output.pos = mul(ubo.proj, mul(ubo.view, mul(ubo.model, input.inPosition)));
  output.color = float4(controls.m_sandColor, 1.0);
  return output;
}