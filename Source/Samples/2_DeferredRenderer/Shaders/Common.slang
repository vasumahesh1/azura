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
  float3 normal : PS_NORMAL;
  float2 uv : PS_UV;
};

struct TextureBlock {
  Texture2D tex;
  Texture2D norm;
};

struct SamplerBlock {
  SamplerState samp;
};

