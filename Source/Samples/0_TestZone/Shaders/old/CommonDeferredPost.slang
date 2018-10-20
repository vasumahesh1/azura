struct VertexShaderInput
{
  float4 in_Pos : POSITION;
  float2 in_UV : UV;
};

struct PixelShaderInput
{
  float4 pos : SV_Position;
  float2 uv : PS_UV;
};

struct UBOData {
  float4x4 u_model;
  float4x4 u_modelInvTranspose;
  float4x4 u_viewProj;
};

struct SamplerBlock {
  SamplerState g_sampler;
};

struct TextureBlock {
  Texture2D g_txDiffuse;
};

struct AttachmentBlock1 {
  Texture2D g_attachmentTexture;
};

ParameterBlock<SamplerBlock> samplerBlock;
ParameterBlock<AttachmentBlock1> attachmentBlock;