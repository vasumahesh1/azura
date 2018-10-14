struct UBOData {
  float4x4 u_model;
  float4x4 u_modelInvTranspose;
  float4x4 u_viewProj;
  float4x4 u_invViewProj;
};

struct ShaderControls {
  float u_shoreLevel;
  float u_elevation;
  float u_noiseScale;
  float pad1;

  float4 u_lightPos;
  float4 u_eye;

  // X = Level, Y = Opacity
  float4 u_waterControls;
  float4 u_waterColor;
};

struct VertexShaderInput
{
  float4 in_Pos : IN_POSITION;
  float4 in_Normal : IN_NORMAL;
};

struct SkyInput_VS
{
  float4 in_Pos : IN_POSITION;
  float4 in_UV : IN_UV;
};

struct SkyInput_PS
{
  float4 pos : SV_Position;
  float4 uv : UV;
};

struct PixelShaderInput
{
  float4 pos : SV_Position;
  float4 normal : NORMAL;
  float4 modelPos : MODEL_POSITION;
  float2 textureOffset : TEXTURE_OFFSET;
};

struct TextureState {
  SamplerState linearSampler;
  Texture2D target;
};

static const float2 ROCK_OFFSET = float2(0.5, 0.0);
static const float2 SNOW_OFFSET = float2(0.0, 0.0);
static const float2 GRASS_1_OFFSET = float2(0.25, 0.0);
static const float2 GRASS_2_OFFSET = float2(0.75, 0.0);
static const float2 BEDROCK_1_OFFSET = float2(0.25, 0.25);
static const float2 BEDROCK_2_OFFSET = float2(0.5, 0.25);
static const float2 SAND_OFFSET = float2(0.0, 0.25);

ParameterBlock<UBOData> ubo;
ParameterBlock<ShaderControls> controls;
ParameterBlock<TextureState> textureState;
