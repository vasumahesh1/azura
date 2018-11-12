#pragma once
#include <memory>
#include <set>
#include "Generic/Renderer.h"
#include "Generic/Window.h"
#include "Memory/RangeAllocator.h"
#include "Memory/HeapMemoryBuffer.h"

#include "Log/Log.h"
#include "Math/Core.h"
#include "Generic/TextureManager.h"
#include "Camera/PolarCamera.h"
#include "Math/Plane.h"
#include <random>

namespace Azura {

struct EdgeConstraints {
  U32 m_indexA;
  U32 m_indexB;
  float m_restLength;
  float m_invMass1{1.0f};
  float m_invMass2{1.0f};

  bool operator<(const EdgeConstraints& rhs) const
  {
    return std::tie(m_indexA, m_indexB) < std::tie(rhs.m_indexA, rhs.m_indexB);
  }

  bool operator==(const EdgeConstraints& rhs) const
  {
    return (m_indexA == rhs.m_indexA) && (m_indexB == rhs.m_indexB);
  }

  Vector4f ComputeDeltaX1(const std::vector<Vector4f>& currentPositions, float k) const
  {
    const float invMassFactor = m_invMass1 / (m_invMass1 + m_invMass2);
    const Vector4f x12 = currentPositions[m_indexA] - currentPositions[m_indexB];
    return (k * -1.0f * invMassFactor * (x12.Length() - m_restLength)) * x12.Normalized();
  }

  Vector4f ComputeDeltaX2(const std::vector<Vector4f>& currentPositions, float k) const
  {
    const float invMassFactor = m_invMass2 / (m_invMass1 + m_invMass2);
    const Vector4f x12 = currentPositions[m_indexA] - currentPositions[m_indexB];
    return (k * invMassFactor * (x12.Length() - m_restLength)) * x12.Normalized();
  }
};

struct SceneUBO {
  Matrix4f m_model;
  Matrix4f m_modelInvTranspose;
  Matrix4f m_view;
  Matrix4f m_viewProj;
  Matrix4f m_invViewProj;
  Matrix4f m_invProj;
};

struct PassData {
  U32 m_clothId;
  U32 m_sphereId;
  U32 m_sceneUBOSlot;
  U32 m_vertexSlot;
  U32 m_normalSlot;
};

class AppRenderer {
public:
  AppRenderer();

  void Initialize();
  void WindowUpdate(float timeDelta);
  void Run() const;
  void Destroy() const;

private:
  void AddEdgeConstraint(const EdgeConstraints& e);

  Memory::HeapMemoryBuffer m_mainBuffer;

  Memory::RangeAllocator m_mainAllocator;
  Memory::RangeAllocator m_drawableAllocator;

  std::unique_ptr<Renderer> m_renderer{nullptr};
  std::unique_ptr<Window> m_window{nullptr};
  std::unique_ptr<TextureManager> m_textureManager{nullptr};

  PolarCamera m_camera;

  SceneUBO m_clothUBO{};
  SceneUBO m_sphereUBO{};
  DrawablePool* m_mainPool{nullptr};

  PassData m_renderPass{};

  Math::Plane m_clothPlane;
  std::vector<Vector4f> m_clothVertexVel;
  std::vector<Vector4f> m_clothProjectedPos;
  std::vector<EdgeConstraints> m_clothEdgeConstraints;
  std::vector<SizeType> m_clothConstraintsIdx;

  Log log_AppRenderer;
};
} // namespace
