static const uint NUM_LIGHTS = 128;

static const uint BLOCK_SIZE = 16;
static const uint MAX_LIGHTS_PER_CLUSTER = 252;

static const float LIGHT_DT = -2.5;
static const float3 LIGHT_MIN = {-5.0, 0.0, -5.0 };
static const float3 LIGHT_MAX = { 5.0, 15.0, 5.0 };
static const uint SCREEN_WIDTH = 1280;
static const uint SCREEN_HEIGHT = 720;
static const uint TILES_X = 16;
static const uint TILES_Y = 16;
static const uint TILES_Z = 16;

static const uint WIDTH_PER_TILE = SCREEN_WIDTH / TILES_X;
static const uint HEIGHT_PER_TILE = SCREEN_HEIGHT / TILES_Y;

static const uint THREAD_COUNT = BLOCK_SIZE * BLOCK_SIZE;

static const uint LIGHTS_PER_THREAD = (NUM_LIGHTS + THREAD_COUNT - 1) / THREAD_COUNT;

static const float3 AMBIENT_LIGHT_INTENSITY = float3(0.1);

static const uint CLUSTER_PIXEL_HEIGHT = 1 + (MAX_LIGHTS_PER_CLUSTER / 4);

struct UBOData {
  float4x4 u_model;
  float4x4 u_modelInvTranspose;
  float4x4 u_viewProj;
};

struct ForwardVertexInput
{
  float3 in_Pos : POSITION;
  float3 in_Normal : NORMAL;
  float2 in_UV : UV;
};

struct ColorPixelInput
{
  float4 pos : SV_Position;
  float4 worldPos : PS_POS;
  float3 normal : PS_NORMAL;
  float2 uv : PS_UV;
};

struct ForwardPlusComputeOut_PS
{
  float4 target1 : SV_Target0;
  float4 target2 : SV_Target1;
  float4 target3 : SV_Target2;
};

struct TextureBlock {
  Texture2D tex;
  Texture2D norm;
  Texture2D light;
};

struct TextureBlockCompute {
  Texture2D tex;
  Texture2D norm;
};

struct ComputeAttachment {
  Texture2D light;
};

struct ForwardPlusComputeTexture_Pass2 {
  RWTexture2D<float4> lightTexture;
  RWTexture2D<float4> clusterTexture;
};

struct ForwardPlusComputeAttachment_Pass2 {
  Texture2D<float> depthBuffer;
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


struct ForwardPlusComputeAttachment_Pass3 {
  Texture2D gbuffer1;
  Texture2D gbuffer2;
  Texture2D gbuffer3;
  Texture2D light;
  Texture2D clusterTexture;
};

struct SamplerBlock {
  SamplerState samp;
  SamplerState lightSamp;
};

struct ForwardPlusCompute_TextureSamplerBlock {
  SamplerState samp;
};

struct ForwardPlusCompute_LightSamplerBlock {
  SamplerState lightSamp;
};

struct Light {
  float3 position;
  float radius;
  float3 color;
  float _pad;
};

struct LightUBO {
  float4x4 view;
  float4x4 invProj;

  float nearPlane;
  float farPlane;
  float2 _pad1;

  float timeDelta;
  float3 _pad2;
};

struct Plane {
    float3 normal;
    float  dist;
};

struct Frustum {
    Plane planes[4];
};

struct FrustumBlock {
    StructuredBuffer<Frustum> frustums;
};

float cubicGaussian(float h) {
  if (h < 1.0) {
    return 0.25 * pow(2.0 - h, 3.0) - pow(1.0 - h, 3.0);
  } else if (h < 2.0) {
    return 0.25 * pow(2.0 - h, 3.0);
  } else {
    return 0.0;
  }
}

float3 applyNormalMap(float3 geomnor, float3 normap) {
  normap = normap * 2.0 - 1.0;

  float3 up = normalize(float3(0.001, 1, 0.001));
  float3 surftan = normalize(cross(geomnor, up));
  float3 surfbinor = cross(geomnor, surftan);

  return normap.y * surftan + normap.x * surfbinor + normap.z * geomnor;
}