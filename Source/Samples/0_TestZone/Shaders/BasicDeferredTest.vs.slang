import CommonDeferred;

PixelShaderInput main(VertexShaderInput input)
{
  float4 vertexPosition = input.in_Pos;
  float2 vertexUV = input.in_UV;

  PixelShaderInput output;
  output.pos = mul(ubo.u_viewProj, mul(ubo.u_model, vertexPosition));
  output.uv = vertexUV;
  return output;
}