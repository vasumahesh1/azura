import Common;

float4 main(ColorPixelInput input) : SV_Target
{
  float4 color = input.color;
  return color;
}