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

struct ForwardRenderer {
  U32 m_passId;
  U32 m_uboSlot;
  U32 m_texSlot;
  U32 m_normalSlot;
  U32 m_sampSlot;
};

struct UniformBufferData {
  Matrix4f m_model;
  Matrix4f m_modelInvTranspose;
  Matrix4f m_viewProj;
};

struct Vertex {
  float m_pos[4];
  float m_norm[4];
  float m_color[4];
};

class AppRenderer {
public:
  AppRenderer();

  void Initialize();
  void WindowUpdate();
  void Run() const;
  void Destroy() const;

private:
  void LoadAssets() const;

  UniformBufferData m_sceneUBO;

  ForwardRenderer m_forwardRenderer;

  Memory::HeapMemoryBuffer m_mainBuffer;

  Memory::RangeAllocator m_mainAllocator;
  Memory::RangeAllocator m_drawableAllocator;

  PolarCamera m_camera;

  std::unique_ptr<Renderer> m_renderer{nullptr};
  std::unique_ptr<Window> m_window{nullptr};
  std::unique_ptr<TextureManager> m_textureManager{nullptr};

  Log log_AppRenderer;
};
} // namespace
