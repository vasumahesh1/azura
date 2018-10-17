struct UBOData {
  float4x4 u_model;
  float4x4 u_modelInvTranspose;
  float4x4 u_viewProj;
};

struct BasicRenderTestVertexInput
{
  float4 in_Pos : IN_POSITION;
  float4 in_Color : IN_COLOR;
};

struct BasicInstanceTestVertexInput
{
  float4 in_Pos : IN_POSITION;
  float4 in_Color : IN_COLOR;
  float4 in_InstancePos : IN_INSTANCE_POS;
};

struct BasicTextureTestVertexInput
{
  float4 in_Pos : IN_POSITION;
  float2 in_UV : IN_UV;
};

struct ColorPixelInput
{
  float4 pos : SV_Position;
  float4 color : COLOR;
};

struct UVPixelInput
{
  float4 pos : SV_Position;
  float2 uv : UV;
};
