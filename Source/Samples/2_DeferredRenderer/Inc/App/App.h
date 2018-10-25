#pragma once

#include "Generic/Renderer.h"
#include "Generic/Window.h"
#include "Memory/RangeAllocator.h"
#include "Memory/HeapMemoryBuffer.h"
#include "Camera/FlyThroughCamera.h"
#include "App/LightTexture.h"
#include "App/Common.h"
#include "App/Scene.h"
#include "Forward/ForwardScene.h"
#include "Forward/ForwardComputeScene.h"
#include "ForwardPlus/ForwardPlusComputeScene.h"

namespace Azura
{
class App
{
public:
  App();

  void Initialize();
  void Update(float timeDelta);
  void Run() const;
  void CleanUp();

private:
  void GenerateLights();

  Log log_App;

  Memory::HeapMemoryBuffer m_mainBuffer;

  Memory::MonotonicAllocator m_mainAllocator;
  Memory::MonotonicAllocator m_drawableAllocator;

  Containers::Vector<PointLight> m_lights;

  UniformBufferData m_sceneUBO;
  FlyThroughCamera m_camera;

  MeshObject m_sponza;

  SamplerDesc m_lightSamplerDesc;

  Scene* p_activeScene{nullptr};

  ForwardScene m_forwardScene;
  ForwardComputeScene m_forwardComputeScene;
  ForwardPlusComputeScene m_forwardPlusComputeScene;

  std::unique_ptr<Window> p_window{nullptr};
};


} // namespace Azura