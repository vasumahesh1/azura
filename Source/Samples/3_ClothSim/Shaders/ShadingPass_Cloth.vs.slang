import ShadingPass;

NormalPixelInput main(ClothVertexInput input)
{
  float4 vertexPosition = float4(input.in_Pos, 1.0);
  float3 vertexNormal = normalize(float3(asfloat(input.in_NormalX), asfloat(input.in_NormalY), asfloat(input.in_NormalZ)));
  float3 vertexTangent = normalize(float3(asfloat(input.in_TangentX), asfloat(input.in_TangentY), asfloat(input.in_TangentZ)));

  NormalPixelInput output;
  output.pos = mul(ubo.u_viewProj, mul(ubo.u_model, vertexPosition));
  output.modelPos = mul(ubo.u_model, vertexPosition);
  output.normal = mul(ubo.u_modelInvTranspose, float4(vertexNormal, 0.0)).xyz;
  output.tangent = mul(ubo.u_model, float4(vertexTangent, 0.0)).xyz;
  output.uv = input.in_UV;
  return output;
}
