import Common;
import ClothCommon;

NormalPixelInput main(ClothVertexInput input)
{
  float4 vertexPosition = input.in_Pos;
  float4 vertexNormal = input.in_Normal;

  NormalPixelInput output;
  output.pos = mul(ubo.u_viewProj, mul(ubo.u_model, vertexPosition));
  output.normal = vertexNormal;
  return output;
}
