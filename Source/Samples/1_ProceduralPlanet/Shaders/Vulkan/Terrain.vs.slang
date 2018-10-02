import Common;

ParameterBlock<ShaderControls> controls;
ParameterBlock<UBOData> ubo;

// ----------  Shader Constants  ----------
static const float DEGREE_TO_RAD = 0.0174533;
static const float RAD_TO_DEGREE = 57.2958;
static const float NO_MATCAP = 0.0;
static const float NO_SPEC = 0.0;

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
  float3 d = float3(0.0);
  float3x3 m = float3x3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);

  for (int i = 0; i < octaves; i++) {
    float4 n = noised(x);
    a += b * n.x;
    d += b * mul(m, n.yzw);
    b *= s;
    x = f * mul(m3, x);
    m = f * m3i * m;
  }
  return float4(a, d.x, d.y, d.z);
}
// =====  End of FMB with Analytical Derivative  ======

// =======================================================
// =            Simplex3D by Nikita Miropolskiy            =
// =======================================================*/
float3 random3(float3 c) {
  float j = 4096.0 * sin(dot(c, float3(17.0, 59.4, 15.0)));
  float3 r;
  r.z = frac(512.0 * j);
  j *= .125;
  r.x = frac(512.0 * j);
  j *= .125;
  r.y = frac(512.0 * j);
  return r - 0.5;
}

static const float F3 = 0.3333333;
static const float G3 = 0.1666667;
float snoise(float3 p) {
  float3 s = floor(p + dot(p, float3(F3)));
  float3 x = p - s + dot(s, float3(G3));

  float3 e = step(float3(0.0), x - x.yzx);
  float3 i1 = e * (1.0 - e.zxy);
  float3 i2 = 1.0 - e.zxy * (1.0 - e);

  float3 x1 = x - i1 + G3;
  float3 x2 = x - i2 + 2.0 * G3;
  float3 x3 = x - 1.0 + 3.0 * G3;

  float4 w, d;

  w.x = dot(x, x);
  w.y = dot(x1, x1);
  w.z = dot(x2, x2);
  w.w = dot(x3, x3);

  w = max(0.6 - w, 0.0);

  d.x = dot(random3(s), x);
  d.y = dot(random3(s + i1), x1);
  d.z = dot(random3(s + i2), x2);
  d.w = dot(random3(s + 1.0), x3);

  w *= w;
  w *= w;
  d *= w;

  return dot(d, float4(52.0));
}

float snoiseFractal(float3 m) {
  return 0.5333333 * snoise(m) + 0.2666667 * snoise(2.0 * m) +
         0.1333333 * snoise(4.0 * m) + 0.0666667 * snoise(8.0 * m);
}
// =====  End of Simplex3D by Nikita Miropolskiy  ======


void RenderPlanet(inout float4 vertexPosition, inout float4 vertexNormal, out int vertexTextureID)
{
  const float noiseScale = (controls.u_noiseScale / 0.5) * 3.0;
  const float waterThreshold = controls.u_waterControls.x - 0.5;
  const float shoreThreshold = waterThreshold + (controls.u_shoreLevel / 0.5) * 0.04;
  const float elevation = (0.5 / controls.u_elevation) * 4.0;

  const float4 originalPosition = vertexPosition;
  const float4 originalNormal = vertexNormal;

  const float3 noiseInput = vertexPosition.xyz * noiseScale;

  const float dist = length(vertexPosition.xyz - controls.u_eye.xyz);
  const int LOD = int(8.0 * (1.0 - smoothstep(0.0, 2.5, log(dist)))) + 4;
  const float4 noiseVector = fbmad(noiseInput, LOD);
  const float noiseValue = noiseVector.x;
  const float3 derivative = noiseVector.yzw;

  const bool isVertexWater = noiseValue < waterThreshold;

  vertexPosition = originalPosition + (originalNormal * noiseValue / elevation);

  if (isVertexWater) {
    vertexTextureID = BEDROCK_1_TEXTURE_ID;
    vertexNormal = float4(normalize(vertexNormal.xyz - (derivative * 0.5)), 0);

    // Deep Water
    if (noiseValue < waterThreshold - 0.15) {
      vertexTextureID = BEDROCK_2_TEXTURE_ID;
    }
  }
  else {
    // Grass
    vertexTextureID = GRASS_1_TEXTURE_ID;

    const float grassDarkJitter = snoiseFractal(originalPosition.xyz * 13.0) * 0.1;

    if (noiseValue < shoreThreshold) {
      // Sand
      vertexTextureID = SAND_TEXTURE_ID;
    }
    else if (noiseValue > shoreThreshold + grassDarkJitter) {
      // Grass 2
      vertexTextureID = GRASS_2_TEXTURE_ID;
    }

    vertexNormal = float4(normalize(vertexNormal.xyz - (derivative * 0.36)), 0);
  }
}

PixelShaderInput main(VertexShaderInput input)
{
  float4 vertexPosition = input.in_Pos;
  float4 vertexNormal = input.in_Normal;
  int vertexTextureID;

  RenderPlanet(vertexPosition, vertexNormal, vertexTextureID);

  PixelShaderInput output;

  output.modelPos = mul(ubo.u_model, vertexPosition);
  output.lightVec = controls.u_lightPos - output.modelPos;

  output.pos = mul(ubo.u_viewProj, output.modelPos);
  output.textureID = vertexTextureID;
  output.normal = mul(ubo.u_modelInvTranspose, vertexNormal);
  return output;
}