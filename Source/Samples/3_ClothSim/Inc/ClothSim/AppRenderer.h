#pragma once
#include <memory>

#include "Generic/Renderer.h"
#include "Generic/Window.h"
#include "Memory/RangeAllocator.h"
#include "Memory/HeapMemoryBuffer.h"

#include "Log/Log.h"
#include "Math/Core.h"
#include "Generic/TextureManager.h"
#include "Camera/PolarCamera.h"
#include "Physics/Geometry/ClothPlane.h"
#include "Math/TransformComponent.h"
#include "Physics/Geometry/ClothMesh.h"


namespace Azura {

struct SceneUBO {
  Matrix4f m_model;
  Matrix4f m_modelInvTranspose;
  Matrix4f m_view;
  Matrix4f m_viewProj;
  Matrix4f m_invViewProj;
  Matrix4f m_invProj;
};

struct ClothComputeProperties {
  Vector3i m_minBounding;
  Vector3i m_maxBounding;
  U32 m_numSelfCollisionConstraints;
};

struct SelfCollisionConstraint {
  U32 m_pIdx;
  U32 m_p0;
  U32 m_p1;
  U32 m_p2;

  Vector3f m_normal;
};

struct ComputeUBO
{
  float m_stretchStiffness;
  float m_bendStiffness;
  float m_longRangeStiffness;
  float m_timeDelta;

  U32 m_numStretchConstraints;
  U32 m_numBendConstraints;
  U32 m_numLongRangeConstraints;
  U32 m_numVertices;

  U32 m_numTriangles;
  U32 m_frameCount{0};
  U32 m_pad[2];

  Matrix4f m_clothModelMatrix[16];
};

struct NormalUBO
{
  U32 m_numVertices;
  U32 m_numTriangles;
  float m_pad[2];
};

struct NormalsPassData
{
  U32 m_uboSlot;
  U32 m_passId;
};

struct ComputePassData
{
  U32 m_computeUBOSlot;
  U32 m_pass1;
  U32 m_passInitialize;
  U32 m_passBinning;
  U32 m_passSelfCollisions;
  std::vector<U32> m_passItr1;
  std::vector<U32> m_passItr2;
  U32 m_pass4;
};

struct PassData {
  U32 m_clothId;
  U32 m_sphereId;
  U32 m_planeId;
  U32 m_shehzanId;
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
  Log log_AppRenderer;

  Memory::HeapMemoryBuffer m_mainBuffer;

  Memory::MonotonicAllocator m_mainAllocator;
  Memory::MonotonicAllocator m_localAllocator;
  Memory::MonotonicAllocator m_drawableAllocator;

  std::unique_ptr<Renderer> m_renderer{nullptr};
  std::unique_ptr<Window> m_window{nullptr};
  std::unique_ptr<TextureManager> m_textureManager{nullptr};

  PolarCamera m_camera;

  SceneUBO m_clothUBO{};
  SceneUBO m_sphereUBO{};
  SceneUBO m_planeUBO{};
  SceneUBO m_shehzanUBO{};
  ComputeUBO m_computeUBO{};
  NormalUBO m_normalUBO{};
  DrawablePool* m_mainPool{nullptr};
  DrawablePool* m_spherePool{nullptr};
  DrawablePool* m_planePool{nullptr};
  DrawablePool* m_shehzanPool{nullptr};
  ComputePool* m_computePools[5]{ nullptr, nullptr, nullptr, nullptr, nullptr};

  std::vector<ComputePool*> m_iterativePools;

  PassData m_renderPass{};
  ComputePassData m_computePass{};
  NormalsPassData m_normalsPass{};

  Math::TransformComponent m_clothTransform{};

  Containers::Vector<Math::TransformComponent> m_curtainTransforms;

  Physics::ClothPlane m_clothPlane;
  Physics::ClothMesh m_clothMesh;

  Physics::IClothPhysicsGeometry* p_activeMesh{nullptr};

  int m_rotateFactor{0};
  const float m_rotateStepSize{Math::PI_OVER4};
};
} // namespace
