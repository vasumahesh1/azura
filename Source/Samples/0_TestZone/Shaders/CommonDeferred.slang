struct VertexShaderInput
{
  float4 in_Pos : IN_POSITION;
  float2 in_UV : IN_UV;
};

struct PixelShaderInput
{
  float4 pos : SV_Position;
  float2 uv : UV;
};
