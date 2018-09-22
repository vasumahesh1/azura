import Common;

ParameterBlock<UBOData> ubo;

PixelShaderInput main(VertexShaderInput input)
{
  PixelShaderInput output;

  float4x4 instanceModel = float4x4(
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0
  );

  instanceModel[0][3] = input.instancePos.x;
  instanceModel[1][3] = input.instancePos.y;
  instanceModel[2][3] = input.instancePos.z;
  instanceModel[3][3] = input.instancePos.w;

  output.pos = mul(ubo.proj, mul(ubo.view, mul(instanceModel, mul(ubo.model, input.inPosition))));
  output.color = input.inColor;

  return output;
}