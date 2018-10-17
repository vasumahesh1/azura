import Common;

ParameterBlock<UBOData> ubo;

ColorPixelInput main(BasicRenderTestVertexInput input)
{
  float4 vertexPosition = input.in_Pos;
  float4 vertexColor = input.in_Color;

  ColorPixelInput output;
  output.pos = mul(ubo.u_viewProj, mul(ubo.u_model, vertexPosition));
  output.color = vertexColor;
  return output;
}