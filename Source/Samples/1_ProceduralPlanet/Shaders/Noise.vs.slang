import Common;
import Simplex3D;

// ----------  Shader Constants  ----------
static const float DEGREE_TO_RAD = 0.0174533;
static const float RAD_TO_DEGREE = 57.2958;

// ======================================================
//             FMB with Analytical Derivative           
// ======================================================
float hash1(float n) { return frac(n * 17.0 * frac(n * 0.3183099)); }

float4 noised(in float3 x) {
  float3 p = floor(x);
  float3 w = frac(x);

  float3 u = w * w * w * (w * (w * 6.0 - 15.0) + 10.0);
  float3 du = 30.0 * w * w * (w * (w - 2.0) + 1.0);

  float n = p.x + 317.0 * p.y + 157.0 * p.z;

  float a = hash1(n + 0.0);
  float b = hash1(n + 1.0);
  float c = hash1(n + 317.0);
  float d = hash1(n + 318.0);
  float e = hash1(n + 157.0);
  float f = hash1(n + 158.0);
  float g = hash1(n + 474.0);
  float h = hash1(n + 475.0);

  float k0 = a;
  float k1 = b - a;
  float k2 = c - a;
  float k3 = e - a;
  float k4 = a - b - c + d;
  float k5 = a - c - e + g;
  float k6 = a - b - e + f;
  float k7 = -a + b + c - d + e - f - g + h;

  float3 tmp = 2.0 * du *
          float3(k1 + k4 * u.y + k6 * u.z + k7 * u.y * u.z,
               k2 + k5 * u.z + k4 * u.x + k7 * u.z * u.x,
               k3 + k6 * u.x + k5 * u.y + k7 * u.x * u.y);

  return float4(
      -1.0 + 2.0 * (k0 + k1 * u.x + k2 * u.y + k3 * u.z + k4 * u.x * u.y +
                    k5 * u.y * u.z + k6 * u.z * u.x + k7 * u.x * u.y * u.z),
      tmp.x, tmp.y, tmp.z
      );
}

static const float3x3 m3 = float3x3(
 0.00, -0.80, -0.60,
 0.80, 0.36, -0.48,
 0.60, -0.48, 0.64
);

static const float3x3 m3i = float3x3(
  0.00, 0.80, 0.60,
  -0.80, 0.36, -0.48,
  -0.60, -0.48, 0.64
);

float4 fbmad(in float3 x, int octaves) {
  float f = 1.98;
  float s = 0.49;
  float a = 0.0;
  float b = 0.5;
  float3 d = float3(0.0, 0.0, 0.0);
  float3x3 m = float3x3(
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0
  );

  for (int i = 0; i < octaves; i++) {
    float4 n = noised(x);
    a += b * n.x;
    d += b * mul(m, n.yzw);

    b *= s;

    x = f * mul(m3, x);
    m = f * mul(m3i, m);
  }
  return float4(a, d.x, d.y, d.z);
}
// =====  End of FMB with Analytical Derivative  ======

void RenderPlanet(inout float4 vertexPosition, inout float4 vertexNormal, out float2 baseUVOffset)
{
  const float noiseScale = (controls.u_noiseScale / 0.5) * 3.0;
  const float waterThreshold = controls.u_waterControls.x - 0.5;
  const float shoreThreshold = waterThreshold + (controls.u_shoreLevel / 0.5) * 0.04;
  const float elevation = (0.5 / controls.u_elevation) * 5.0;

  const float4 originalPosition = vertexPosition;
  const float4 originalNormal = vertexNormal;

  const float3 noiseInput = vertexPosition.xyz * noiseScale;

  const float dist = length(vertexPosition.xyz - controls.u_eye.xyz);
  const int LOD = int(8.0 * (1.0 - smoothstep(0.0, 2.5, log(dist)))) + 4;
  const float4 noiseVector = fbmad(noiseInput, LOD);
  const float noiseValue = noiseVector.x;
  const float3 derivative = noiseVector.yzw;

  const bool isVertexWater = noiseValue < waterThreshold;

  vertexNormal = float4(normalize(vertexNormal.xyz - (derivative * 0.36)), 0);
  vertexPosition = originalPosition + float4(originalNormal.xyz * (noiseValue / elevation), 0.0);

  if (isVertexWater) {
    baseUVOffset = BEDROCK_1_OFFSET;
    vertexNormal = float4(normalize(vertexNormal.xyz - (derivative * 0.5)), 0);

    // Deep Water
    if (noiseValue < waterThreshold - 0.15) {
      baseUVOffset = BEDROCK_2_OFFSET;
    }
  }
  else {
    // Grass
    baseUVOffset = GRASS_1_OFFSET;

    const float grassDarkJitter = snoise_fractal(originalPosition.xyz * 13.0) * 0.1;

    if (noiseValue < shoreThreshold) {
      // Sand
      baseUVOffset = SAND_OFFSET;
    }
    else if (noiseValue > shoreThreshold + grassDarkJitter && noiseValue < waterThreshold + 0.3) {
      // Grass 2
      baseUVOffset = GRASS_2_OFFSET;
    }
    else if (noiseValue > waterThreshold + 0.3) {
      float snowAppearance = dot(normalize(derivative), float3(0, 1, 0));
      float snowJitter = snoise_fractal(originalPosition.xyz * 7.0) * 0.05;

      vertexNormal = float4(normalize(originalNormal.xyz - (derivative * 0.45)), 0);

      if (noiseValue > waterThreshold + 0.4 + snowJitter && snowAppearance > 0.5) {
        baseUVOffset = SNOW_OFFSET;
      }
      else {
        baseUVOffset = ROCK_OFFSET; 
      }
    }
  }
}

NoiseInput_PS main(NoiseInput_VS input)
{
  float4 vertexPosition = input.in_Pos;
  float4 vertexNormal = input.in_Normal;
  float2 baseUVOffset;

  RenderPlanet(vertexPosition, vertexNormal, baseUVOffset);

  NoiseInput_PS output;

  output.originalPos = mul(ubo.u_model, input.in_Pos);
  output.modelPos = mul(ubo.u_model, vertexPosition);
  output.pos = mul(ubo.u_viewProj, output.modelPos);
  output.normal = mul(ubo.u_modelInvTranspose, vertexNormal);
  output.baseUVOffset = baseUVOffset;
  return output;
}
