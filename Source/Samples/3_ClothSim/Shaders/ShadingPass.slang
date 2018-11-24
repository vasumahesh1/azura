struct NormalPixelInput
{
  float4 pos : SV_Position;
  float4 modelPos : PS_MODELPOS;
  float3 normal : PS_NORMAL;
};

struct ClothVertexInput
{
  float3 in_Pos : POSITION;
  uint in_NormalX : NORMALX;
  uint in_NormalY : NORMALY;
  uint in_NormalZ : NORMALZ;
};

struct SceneUBO {
  float4x4 u_model;
  float4x4 u_modelInvTranspose;
  float4x4 u_view;
  float4x4 u_viewProj;
  float4x4 u_invViewProj;
  float4x4 u_invProj;
};

struct LightData {
  float4 u_pos;
};

ParameterBlock<SceneUBO> ubo;
ParameterBlock<LightData> light;