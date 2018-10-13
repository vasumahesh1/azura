import CommonDeferred;

PixelShaderInput main(VertexShaderInput input)
{
  PixelShaderInput output;
  output.pos = input.in_Pos;
  output.uv = input.in_UV;
  return output;
}