import Common;

QuadInput_PS main(QuadInput_VS input)
{
  QuadInput_PS output;
  output.pos = input.in_Pos;
  output.uv = input.in_UV;
  return output;
}
