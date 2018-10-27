#pragma once

#include "Math/Core.h"
#include "AssetManager/GLTFLoader.h"

namespace Azura {

constexpr U32 NUM_LIGHTS = 128;
constexpr U32 MAX_LIGHTS_PER_CLUSTER = 252;
const Vector3f LIGHT_MIN = {-5.0f, 0.0f, -5.0f };
const Vector3f LIGHT_MAX = { 5.0f, 15.0f, 5.0f };
constexpr float LIGHT_RADIUS = 5.0f;
constexpr float LIGHT_DT = -2.5f;

constexpr U32 TILES_X = 16;
constexpr U32 TILES_Y = 16;
constexpr U32 TILES_Z = 16;

constexpr U32 CLUSTER_PIXEL_HEIGHT = 1 + (MAX_LIGHTS_PER_CLUSTER / 4);

constexpr U32 FPLUS_NUM_FRUSTRUMS = TILES_X * TILES_Y;

struct UniformBufferData {
  Matrix4f m_model;
  Matrix4f m_modelInvTranspose;
  Matrix4f m_viewProj;
};

struct PointLight {
  Vector3f m_position;
  float m_radius;

  Vector3f m_color;
  float _pad;
};

struct MeshObject
{
  std::unique_ptr<GLTFMeshInterface> p_interface{nullptr};

  std::vector<float> m_vertexData{};
  std::vector<float> m_normalData{};
  std::vector<float> m_uvData{};
  std::vector<U32> m_indexData{};

  U32 m_numVertices{0};
  U32 m_vertexDataSize{0};

  U32 m_numIndices{0};
  U32 m_indexDataSize{0};

  U32 m_numUV{0};
  U32 m_uvDataSize{0};

  U32 m_numNormals{0};
  U32 m_normalDataSize{0};
};

struct LightUBO
{
  Matrix4f m_view{};

  float m_nearPlane{0.0f};
  float m_farPlane{0.0f};
  float m_timeDelta{ 0.0f };
  float _pad1[1];

  Matrix4f m_proj{};
};

struct Plane
{
  Vector3f m_normal;
  float  m_distanceToOrigin;
};

struct Frustum
{
  Plane m_planes[4];
};


} // namespace Azura