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
#include "TestZone/ClothMesh.h"
#include "TestZone/ImportedClothMesh.h"

namespace Azura {
constexpr U32 BLOCK_SIZE_X = 512;
constexpr U32 SOLVER_ITERATIONS = 32;

struct SceneUBO {
  Matrix4f m_model;
  Matrix4f m_modelInvTranspose;
  Matrix4f m_view;
  Matrix4f m_viewProj;
  Matrix4f m_invViewProj;
  Matrix4f m_invProj;
};

struct ComputeUBO
{
  float m_stretchStiffness;
  float m_bendStiffness;
  float m_timeDelta;
  U32 m_numStretchConstraints;
  
  U32 m_numBendConstraints;
  U32 m_numVertices;
  U32 m_numBlocks;
  float pad;
};

struct ComputePassData
{
  U32 m_computeUBOSlot;
  U32 m_passId;
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
  Memory::HeapMemoryBuffer m_mainBuffer;

  Memory::RangeAllocator m_mainAllocator;
  Memory::RangeAllocator m_drawableAllocator;

  std::unique_ptr<Renderer> m_renderer{nullptr};
  std::unique_ptr<Window> m_window{nullptr};
  std::unique_ptr<TextureManager> m_textureManager{nullptr};

  PolarCamera m_camera;

  SceneUBO m_clothUBO{};
  SceneUBO m_sphereUBO{};
  ComputeUBO m_computeUBO{};
  DrawablePool* m_mainPool{nullptr};
  DrawablePool* m_spherePool{nullptr};
  ComputePool* m_computePool{nullptr};

  PassData m_renderPass{};
  ComputePassData m_computePass{};

  ClothMesh m_clothPlane;
  std::vector<Vector4f> m_clothVertexVel;
  std::vector<Vector4f> m_clothProjectedPos;
  std::vector<SizeType> m_clothConstraintsIdx;

  Log log_AppRenderer;
};
} // namespace
