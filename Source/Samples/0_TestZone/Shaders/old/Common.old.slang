struct UBOData {
  float4x4 u_model;
  float4x4 u_modelInvTranspose;
  float4x4 u_viewProj;
};

struct Controls {
  float4 color;
};

struct VertexShaderInput
{
  float4 in_Pos : IN_POSITION;
  float4 in_Color : IN_COLOR;
  float2 in_UV : IN_UV;
};

struct PixelShaderInput
{
  float4 pos : SV_Position;
  float4 color : COLOR;
  float2 uv : UV;
};

struct SamplerBlock {
  SamplerState g_sampler;
  Texture2D g_txDiffuse;
};

ParameterBlock<UBOData> ubo;
ParameterBlock<SamplerBlock> samplerBlock;