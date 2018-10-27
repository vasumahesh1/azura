#pragma once
#include <memory>
#include "Generic/Renderer.h"
#include "Generic/Window.h"
#include "Memory/MonotonicAllocator.h"
#include "Memory/HeapMemoryBuffer.h"

#include "Log/Log.h"
#include "Math/Core.h"
#include "Generic/TextureManager.h"
#include "Camera/PolarCamera.h"

namespace Azura {
struct AppPass1
{
  U32 m_uboSlot{0};
};

struct UniformBufferData {
  Matrix4f m_model;
  Matrix4f m_modelInvTranspose;
  Matrix4f m_viewProj;
  Matrix4f m_invViewProj;
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

  Memory::MonotonicAllocator m_mainAllocator;
  Memory::MonotonicAllocator m_drawableAllocator;

  std::unique_ptr<Renderer> m_renderer{nullptr};
  std::unique_ptr<Window> m_window{nullptr};
  std::unique_ptr<TextureManager> m_textureManager{nullptr};

  PolarCamera m_camera;

  UniformBufferData m_sceneUBO;

  U32 m_icosphereId{0};

  AppPass1 m_pass1{};
  DrawablePool* m_mainPool{nullptr};
  DrawablePool* m_skyPool{nullptr};

  Log log_AppRenderer;
};
} // namespace
