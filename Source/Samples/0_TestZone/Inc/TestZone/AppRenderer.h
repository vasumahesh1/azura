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

namespace Azura {

  struct SceneUBO {
    Matrix4f m_model;
    Matrix4f m_modelInvTranspose;
    Matrix4f m_viewProj;
    Matrix4f m_invViewProj;
    Matrix4f m_invProj;
  };

  struct PassData
  {
    U32 m_clothId;
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

  SceneUBO m_sceneUBO{};
  DrawablePool* m_mainPool{nullptr};

  PassData m_renderPass{};


  Log log_AppRenderer;
};
} // namespace
