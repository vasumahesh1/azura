import ShadingPass;

NormalPixelInput main(ClothVertexInput input)
{
  float4 vertexPosition = float4(input.in_Pos, 1.0);
  float3 vertexNormal = input.in_Normal;

  NormalPixelInput output;
  output.pos = mul(ubo.u_viewProj, mul(ubo.u_model, vertexPosition));
  output.modelPos = mul(ubo.u_model, vertexPosition);
  output.normal = mul(ubo.u_modelInvTranspose, float4(vertexNormal, 0.0)).xyz;
  return output;
}
