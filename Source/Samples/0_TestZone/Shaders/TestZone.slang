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

ParameterBlock<UBOData> ubo;
SamplerState     textureSampler;
Texture2D        textureSource;

PixelShaderInput vsMain(VertexShaderInput input)
{
  float4 vertexPosition = input.in_Pos;
  float4 vertexColor = input.in_Color;
  float2 vertexUV = input.in_UV;

  PixelShaderInput output;
  output.pos = mul(ubo.u_viewProj, mul(ubo.u_model, vertexPosition));
  output.uv = vertexUV;
  output.color = vertexColor;
  return output;
}

float4 psMain(PixelShaderInput input) : SV_Target
{
  float4 color = float4(textureSource.Sample(textureSampler, input.uv).rgb, 1.0);
  return color;
}