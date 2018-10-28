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
  Matrix4f m_view;
  Matrix4f m_proj;
  Matrix4f m_invViewProj;
  Matrix4f m_invProj;
};


struct ShaderControls {
  // NOLINT
  float m_shoreLevel{0.5f};
  float m_elevation{0.5f};
  float m_noiseScale{0.5f};
  float pad1;

  Vector4f m_lightPos{0.0f, 0.0f, 15.0f, 1.0f};
  Vector4f m_eye{0.0f, 0.0f, 4.0f, 1.0f};

  Color4f m_waterControls{0.5f, 0.65f, 0, 0};
  Color4f m_waterColor{21.0f / 255.0f, 92.0f / 255.0f, 158.0f / 255.0f, 1.0f};
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
  ShaderControls m_sceneControls{};

  U32 m_icosphereId{0};

  AppPass1 m_pass1{};
  DrawablePool* m_mainPool{nullptr};
  DrawablePool* m_skyPool{nullptr};
  DrawablePool* m_terrainPool{nullptr};
  DrawablePool* m_waterPool{nullptr};

  Log log_AppRenderer;
};
} // namespace
