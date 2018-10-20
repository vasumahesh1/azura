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
  float4 in_Pos : POSITION;
  float4 in_Normal : NORMAL;
};

struct NoiseInput_VS
{
  float4 in_Pos : POSITION;
  float4 in_Normal : NORMAL;
};

struct NoiseInput_PS
{
  float4 pos : SV_Position;
  float4 normal : NORMAL;
  float4 originalPos : ORIGINAL_POSITION;
  float4 modelPos : MODEL_POSITION;
  float2 baseUVOffset : UV_OFFSET;
};

struct NoiseOuput_PS
{
  float4 target1 : SV_Target0;
  float4 target2 : SV_Target1;
  float4 target3 : SV_Target2;
};

struct QuadInput_VS
{
  float4 in_Pos : POSITION;
  float2 in_UV : UV;
};

struct QuadInput_PS
{
  float4 pos : SV_Position;
  float2 uv : UV;
};

struct PixelShaderInput
{
  float4 pos : SV_Position;
  float4 normal : NORMAL;
  float4 modelPos : MODEL_POSITION;
  float2 textureOffset : TEXTURE_OFFSET;
};

struct TextureState {
  Texture2D target;
};

struct SamplerBlock {
  SamplerState linearSampler;
};

struct AttachmentState {
  Texture2D target1;
  Texture2D target2;
  Texture2D target3;
};

static const float2 ROCK_OFFSET = float2(0.5, 0.0);
static const float2 SNOW_OFFSET = float2(0.0, 0.0);
static const float2 GRASS_1_OFFSET = float2(0.25, 0.0);
static const float2 GRASS_2_OFFSET = float2(0.75, 0.0);
static const float2 BEDROCK_1_OFFSET = float2(0.25, 0.25);
static const float2 BEDROCK_2_OFFSET = float2(0.5, 0.25);
static const float2 SAND_OFFSET = float2(0.0, 0.25);

