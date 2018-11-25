import ShadingPass;

SpherePixelInput main(SphereVertexInput input)
{
  float4 vertexPosition = input.in_Pos;
  float3 vertexNormal = input.in_Normal;

  SpherePixelInput output;
  output.pos = mul(ubo.u_viewProj, mul(ubo.u_model, vertexPosition));
  output.modelPos = mul(ubo.u_model, vertexPosition);
  output.normal = mul(ubo.u_modelInvTranspose, float4(vertexNormal, 0.0)).xyz;
  return output;
}
