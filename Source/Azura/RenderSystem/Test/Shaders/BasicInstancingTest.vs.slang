import Common;

ParameterBlock<UBOData> ubo;

ColorPixelInput main(BasicInstanceTestVertexInput input)
{
  float4 vertexPosition = input.in_Pos;
  float4 vertexColor = input.in_Color;
  float4 vertexInstancePos = input.in_InstancePos;

  float4x4 instanceModel = float4x4(
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
  );

  instanceModel[0][3] = vertexInstancePos.x;
  instanceModel[1][3] = vertexInstancePos.y;
  instanceModel[2][3] = vertexInstancePos.z;

  ColorPixelInput output;
  output.pos = mul(ubo.u_viewProj, mul(instanceModel, mul(ubo.u_model, vertexPosition)));
  output.color = vertexColor;
  return output;
}