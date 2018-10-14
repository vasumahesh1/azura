import Common;

NoiseOuput_PS main(NoiseInput_PS input) : SV_Target
{
  NoiseOuput_PS result;

  float4 vertexPosition = input.modelPos;
  float4 vertexNormal = input.normal;
  float2 baseUVOffset = input.baseUVOffset;

  float elevation = length(input.modelPos);
  float originalElevation = length(input.originalPos);
  float diff = elevation - originalElevation;

  result.target1 = float4(elevation, baseUVOffset.x, baseUVOffset.y, diff);
  result.target2 = float4(vertexPosition.xyz, 1.0);
  result.target3 = float4(vertexNormal.xyz, 0.0);

  return result;
}
