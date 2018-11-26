struct NormalPixelInput
{
  float4 pos : SV_Position;
  float4 modelPos : PS_MODELPOS;
  float3 normal : PS_NORMAL;
  float3 tangent : PS_TANGENT;
  float2 uv : PS_UV;
};

struct SpherePixelInput
{
  float4 pos : SV_Position;
  float4 modelPos : PS_MODELPOS;
  float3 normal : PS_NORMAL;
};

struct PlanePixelInput
{
  float4 pos : SV_Position;
  float4 modelPos : PS_MODELPOS;
  float3 normal : PS_NORMAL;
  float2 uv : PS_UV;
};

struct ClothVertexInput
{
  float3 in_Pos : POSITION;
  float2 in_UV : UV;
  uint in_NormalX : NORMALX;
  uint in_NormalY : NORMALY;
  uint in_NormalZ : NORMALZ;
  uint in_TangentX : TANGENTX;
  uint in_TangentY : TANGENTY;
  uint in_TangentZ : TANGENTZ;
};

struct SphereVertexInput
{
  float4 in_Pos : POSITION;
  float3 in_Normal : NORMAL;
};

struct PlaneVertexInput
{
  float4 in_Pos : POSITION;
  float3 in_Normal : NORMAL;
  float2 in_UV : UV;
};

struct SceneUBO {
  float4x4 u_model;
  float4x4 u_modelInvTranspose;
  float4x4 u_view;
  float4x4 u_viewProj;
  float4x4 u_invViewProj;
  float4x4 u_invProj;
};

struct LightData {
  float4 u_pos;
};

struct TextureBlock {
  Texture2D albedo;
  Texture2D normal;
  Texture2D roughness;
  Texture2D ao;
};

struct SamplerBlock {
  SamplerState linearSampler;
};

struct Attributes
{
  float3 position;
  float2 uv;
  float3 normal;
  float3 binormal;
  float3 tangent;
};

struct Light
{
  float4 color;
  float3 position;
  float p0;
  float3 direction;
  float p1;
  float3 lightVector;
  float intensity;
};

struct Material
{
  float4 albedo;
  float3 specular;
  float roughness;
};

const static float AMBIENT_TERM = 0.3;
const static float GAMMA = 2.2;
const static float PI = 3.1415926535897932384626433832795;

// 
// Unreal 4 Real Shading
// References:
// 1. Original PBR Paper of Unreal
// 2. Sample Code: github.com/TheCherno/Sparky/blob/master/Sandbox/shaders/AdvancedLighting.hlsl
// Still needed some tweaking
//
float FresnelSchlick(float f0, float fd90, float view)
{
  return f0 + (fd90 - f0) * pow(max(1.0f - view, 0.1f), 5.0f);
}

float Disney(Attributes attributes, Light light, Material material, float3 eye)
{
  float3 halfVector = normalize(light.lightVector + eye);

  float NdotL = saturate(dot(attributes.normal, light.lightVector));
  float LdotH = saturate(dot(light.lightVector, halfVector));
  float NdotV = saturate(dot(attributes.normal, eye));

  float energyBias = lerp(0.0f, 0.5f, material.roughness);
  float energyFactor = lerp(1.0f, 1.0f / 1.51f, material.roughness);
  float fd90 = energyBias + 2.0f * (LdotH * LdotH) * material.roughness;
  float f0 = 1.0f;

  float lightScatter = FresnelSchlick(f0, fd90, NdotL);
  float viewScatter = FresnelSchlick(f0, fd90, NdotV);

  return lightScatter * viewScatter * energyFactor;
}

float3 GGX(Attributes attributes, Light light, Material material, float3 eye)
{
  float3 h = normalize(light.lightVector + eye);
  float NdotH = saturate(dot(attributes.normal, h));

  float rough2 = max(material.roughness * material.roughness, 2.0e-3f); // capped so spec highlights don't disappear
  float rough4 = rough2 * rough2;

  float d = (NdotH * rough4 - NdotH) * NdotH + 1.0f;
  float D = rough4 / (PI * (d * d));

  // Fresnel
  float3 reflectivity = material.specular;
  float fresnel = 1.0;
  float NdotL = saturate(dot(attributes.normal, light.lightVector));
  float LdotH = saturate(dot(light.lightVector, h));
  float NdotV = saturate(dot(attributes.normal, eye));
  float3 F = reflectivity + (fresnel - fresnel * reflectivity) * exp2((-5.55473f * LdotH - 6.98316f) * LdotH);

  // geometric / visibility
  float k = rough2 * 0.5f;
  float G_SmithL = NdotL * (1.0f - k) + k;
  float G_SmithV = NdotV * (1.0f - k) + k;
  float G = 0.25f / (G_SmithL * G_SmithV);

  return G * D * F;
}

float3 FinalGamma(float3 color)
{
  return pow(color, 1.0 / GAMMA);
}


ParameterBlock<SceneUBO> ubo;
ParameterBlock<LightData> light;
ParameterBlock<SamplerBlock> samplerBlock;
ParameterBlock<TextureBlock> textureBlock;
