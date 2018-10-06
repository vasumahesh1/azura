import Common;

float4 main(PixelShaderInput input) : SV_Target
{
  float4 color = input.color;
  return color;
}