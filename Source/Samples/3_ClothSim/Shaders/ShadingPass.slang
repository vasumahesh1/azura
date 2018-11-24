struct NormalPixelInput
{
  float4 pos : SV_Position;
  float4 modelPos : PS_MODELPOS;
  float3 normal : PS_NORMAL;
  float3 tangent : PS_TANGENT;
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

ParameterBlock<SceneUBO> ubo;
ParameterBlock<LightData> light;
ParameterBlock<SamplerBlock> samplerBlock;
ParameterBlock<TextureBlock> textureBlock;