import Common;

SkyInput_PS main(SkyInput_VS input)
{
  SkyInput_PS output;
  output.pos = input.in_Pos;
  output.uv = input.in_UV;
  return output;
}