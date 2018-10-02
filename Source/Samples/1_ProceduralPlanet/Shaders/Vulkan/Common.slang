struct UBOData {
  float4x4 u_model;
  float4x4 u_modelInvTranspose;
  float4x4 u_viewProj;
};

struct ShaderControls {
  float u_shoreLevel;
  float u_elevation;
  float u_noiseScale;
  float pad1;

  float4 u_sandColor;
  float4 u_waterBedrock1Color;
  float4 u_waterBedrock2Color;

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

struct PixelShaderInput
{
  float4 pos : SV_Position;
  float4 normal : NORMAL;
  float4 modelPos : MODEL_POSITION;
  float4 lightVec : LIGHT_VEC;
  int textureID : TEXTURE_ID;
};

static const int ROCK_TEXTURE_ID = 1;
static const int SNOW_TEXTURE_ID = 2;
static const int GRASS_1_TEXTURE_ID = 3;
static const int GRASS_2_TEXTURE_ID = 4;
static const int BEDROCK_1_TEXTURE_ID = 5;
static const int BEDROCK_2_TEXTURE_ID = 6;
static const int SAND_TEXTURE_ID = 7;
