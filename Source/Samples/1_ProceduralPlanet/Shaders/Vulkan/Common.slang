struct UBOData {
  float4x4 model;
  float4x4 view;
  float4x4 proj;
};

struct VertexShaderInput
{
  float4 inPosition : POSITION;
  float4 inColor : COLOR;
  float4 instancePos : INSTANCE_POS;
};

struct PixelShaderInput
{
  float4 pos : SV_Position;
  float4 color : COLOR;
};
