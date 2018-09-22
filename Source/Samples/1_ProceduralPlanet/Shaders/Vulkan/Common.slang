struct UBOData {
  float4x4 model;
  float4x4 view;
  float4x4 proj;
};

struct ShaderControls {
  float m_shoreLevel;
  float m_elevation;
  float m_noiseScale;

  float3 m_sandColor;
  float3 m_waterBedrock1Color;
  float3 m_waterBedrock2Color;
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
