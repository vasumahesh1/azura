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

namespace Azura {

struct EdgeConstraints {
  U32 m_indexA;
  U32 m_indexB;
  float m_restLength;

  bool operator<(const EdgeConstraints& rhs) const
  {
    return std::tie(m_indexA, m_indexB) < std::tie(rhs.m_indexA, rhs.m_indexB);
  }

  Vector4f ComputeDeltaX1(const std::vector<Vector4f>& currentPositions) const
  {
    const Vector4f x12 = currentPositions[m_indexA] - currentPositions[m_indexB];
    return -0.5f * (x12.Length() - m_restLength) * x12.Normalized();
  }

  Vector4f ComputeDeltaX2(const std::vector<Vector4f>& currentPositions) const
  {
    const Vector4f x12 = currentPositions[m_indexA] - currentPositions[m_indexB];
    return 0.5f * (x12.Length() - m_restLength) * x12.Normalized();
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
};

class AppRenderer {
public:
  AppRenderer();

  void Initialize();
  void WindowUpdate(float timeDelta);
  void Run() const;
  void Destroy() const;

private:
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
  std::set<EdgeConstraints> m_clothEdgeConstraints;

  Log log_AppRenderer;
};
} // namespace
