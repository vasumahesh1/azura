#pragma once

#include "Math/Core.h"
#include "AssetManager/GLTFLoader.h"

namespace Azura {

constexpr U32 NUM_LIGHTS = 128;
const Vector3f LIGHT_MIN = {-5.0f, 0.0f, -5.0f };
const Vector3f LIGHT_MAX = { 5.0f, 15.0f, 5.0f };
constexpr float LIGHT_RADIUS = 5.0f;
constexpr float LIGHT_DT = -2.5f;

struct UniformBufferData {
  Matrix4f m_model;
  Matrix4f m_modelInvTranspose;
  Matrix4f m_viewProj;
};

struct PointLight {
  Vector3f m_position;
  float m_radius;

  Vector3f m_color;
  float pad;
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

} // namespace Azura