struct UBOData {
  float4x4 model;
  float4x4 view;
  float4x4 proj;
};

struct VertexShaderInput
{
  float4 inPosition : POSITION;
};

struct PixelShaderInput
{
  float4 pos : SV_Position;
  float4 color : COLOR;
};
