const static int NUM_LIGHTS = 128;

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

struct ForwardPlusComputeOut_PS
{
  float4 target1 : SV_Target0;
  float4 target2 : SV_Target1;
};

struct TextureBlock {
  Texture2D tex;
  Texture2D norm;
  Texture2D light;
};

struct TextureBlockCompute {
  Texture2D tex;
  Texture2D norm;
};

struct ComputeAttachment {
  Texture2D light;
};

struct ForwardPlusComputeTexture_Pass2 {
  RWTexture2D<float4> lightTexture;
  // RWTexture2D<float4> clusterTexture;
};

struct ForwardPlusComputeAttachment_Pass2 {
  Texture2D<float> depthBuffer;
};

struct QuadInput_VS
{
  float4 in_Pos : POSITION;
  float2 in_UV : UV;
};

struct QuadInput_PS
{
  float4 pos : SV_Position;
  float2 uv : UV;
};


struct ForwardPlusComputeAttachment_Pass3 {
  Texture2D gbuffer1;
  Texture2D gbuffer2;
  Texture2D light;
  // Texture2D clusterTexture;
};

struct SamplerBlock {
  SamplerState samp;
  SamplerState lightSamp;
};

struct ForwardPlusCompute_TextureSamplerBlock {
  SamplerState samp;
};

struct ForwardPlusCompute_LightSamplerBlock {
  SamplerState lightSamp;
};

struct Light {
  float3 position;
  float radius;
  float3 color;
  float _pad;
};

struct LightUBO {
  float timeDelta;
  float3 _pad;
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

// float ExtractFloat(Texture2D tex, int textureWidth, int textureHeight, int index, int component) {
//   float u = float(index + 1) / float(textureWidth + 1);
//   int pixel = component / 4;
//   float v = float(pixel + 1) / float(textureHeight + 1);

//   float4 texel = tex.Sample(samplerBlock.lightSamp, float2(u, v));

//   int pixelComponent = component - pixel * 4;

//   if (pixelComponent == 0) {
//     return texel[0];
//   } else if (pixelComponent == 1) {
//     return texel[1];
//   } else if (pixelComponent == 2) {
//     return texel[2];
//   } else if (pixelComponent == 3) {
//     return texel[3];
//   }

//   return 0.0;
// }

// Light UnpackLight(int index) {
//   Light light;
//   float u = float(index + 1) / float(NUM_LIGHTS + 1);
  
//   float4 v1 = attachmentBlock.light.Sample(samplerBlock.lightSamp, float2(u, 0.0));
//   float4 v2 = attachmentBlock.light.Sample(samplerBlock.lightSamp, float2(u, 0.5));

//   light.position = v1.xyz;
//   light.radius = ExtractFloat(attachmentBlock.light, NUM_LIGHTS, 2, index, 3);
//   light.color = v2.rgb;
  
//   return light;
// }

float3 applyNormalMap(float3 geomnor, float3 normap) {
  normap = normap * 2.0 - 1.0;

  float3 up = normalize(float3(0.001, 1, 0.001));
  float3 surftan = normalize(cross(geomnor, up));
  float3 surfbinor = cross(geomnor, surftan);

  return normap.y * surftan + normap.x * surfbinor + normap.z * geomnor;
}