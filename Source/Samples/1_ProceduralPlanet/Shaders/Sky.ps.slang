import Common;
import Pass2;
import Perlin3D;

float fbm(float3 x) {
  float v = 0.0;
  float a = 0.5;
  float3 shift = float3(100);

  for (int i = 0; i < 8; ++i) {
    v += a * cnoise(x);
    x = x * 2.0 + shift;
    a *= 0.5;
  }

  return v;
}

float4 GetSkyColor(in float3 rayDir) {
  float noiseValue = fbm(rayDir * 237.5);

  float4 color = float4(0,0,0,1);

  if (noiseValue > 0.5) {
    color = float4(noiseValue, noiseValue, noiseValue, 1.0);
  }

  return color;
}

float4 main(QuadInput_PS input) : SV_Target
{
  // Convert Pixel Coords to 0 to 1 and then to NDC -1 to 1
  float2 ndc = input.uv.xy;

  // Find a point on the Far Plane
  float4 pointInFarPlane = float4(ndc.xy, 1, 1);
  pointInFarPlane *= 100.0;
  float4 worldSpacePoint = mul(ubo.u_invViewProj, pointInFarPlane);

  // Ray from Camera to Far Plane World Point
  float3 rayDir = normalize(worldSpacePoint.xyz - controls.u_eye.xyz);

  float4 targetColor = GetSkyColor(rayDir);

  return targetColor;
}
