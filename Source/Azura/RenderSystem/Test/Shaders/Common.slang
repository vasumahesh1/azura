struct UBOData {
  float4x4 u_model;
  float4x4 u_modelInvTranspose;
  float4x4 u_viewProj;
};

struct BasicRenderTestVertexInput
{
  float4 in_Pos : POSITION;
  float4 in_Color : COLOR;
};

struct BasicInstanceTestVertexInput
{
  float4 in_Pos : POSITION;
  float4 in_Color : COLOR;
  float4 in_InstancePos : INSTANCE_POSITION;
};

struct BasicTextureTestVertexInput
{
  float4 in_Pos : POSITION;
  float2 in_UV : UV;
};

struct ColorPixelInput
{
  float4 pos : SV_Position;
  float4 color : PS_COLOR;
};

struct UVPixelInput
{
  float4 pos : SV_Position;
  float2 uv : UV;
};

struct TextureBlock {
  Texture2D target;
};

struct SamplerBlock {
  SamplerState linearSampler;
};

