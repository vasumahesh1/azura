import CommonDeferred;

PixelShaderInput main(VertexShaderInput input)
{
  PixelShaderInput output;
  output.pos = input.in_Pos;
  return output;
}