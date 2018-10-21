struct UBOData {
  float4x4 u_model;
  float4x4 u_modelInvTranspose;
  float4x4 u_viewProj;
};

struct ForwardVertexInput
{
  float3 in_Pos : POSITION;
  float3 in_Normal : NORMAL;
  float2 in_UV : UV;
};

struct ColorPixelInput
{
  float4 pos : SV_Position;
  float4 worldPos : PS_POS;
  float3 normal : PS_NORMAL;
  float2 uv : PS_UV;
};

struct TextureBlock {
  Texture2D tex;
  Texture2D norm;
  Texture2D light;
};

struct SamplerBlock {
  SamplerState samp;
  SamplerState lightSamp;
};

struct Light {
  float3 position;
  float radius;
  float3 color;
};

float cubicGaussian(float h) {
  if (h < 1.0) {
    return 0.25 * pow(2.0 - h, 3.0) - pow(1.0 - h, 3.0);
  } else if (h < 2.0) {
    return 0.25 * pow(2.0 - h, 3.0);
  } else {
    return 0.0;
  }
}

const static int NUM_LIGHTS = 20;