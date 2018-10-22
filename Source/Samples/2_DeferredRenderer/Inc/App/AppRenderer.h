#pragma once
#include <memory>
#include "Generic/Renderer.h"
#include "Generic/Window.h"
#include "Memory/RangeAllocator.h"
#include "Memory/HeapMemoryBuffer.h"

#include "Log/Log.h"
#include "Math/Core.h"
#include "Generic/TextureManager.h"
#include "Camera/FlyThroughCamera.h"

#include "App/LightTexture.h"

namespace Azura {

struct ForwardRenderer {
  U32 m_passId;

  U32 m_uboSlot;

  U32 m_sampSlot;
  U32 m_lightSampSlot;

  U32 m_texSlot;
  U32 m_normalSlot;
  U32 m_lightTexSlot;
};

struct UniformBufferData {
  Matrix4f m_model;
  Matrix4f m_modelInvTranspose;
  Matrix4f m_viewProj;
};

class AppRenderer {
public:
  AppRenderer();

  void Initialize();
  void WindowUpdate(float timeDelta);
  void Run() const;
  void Destroy() const;

private:
  void LoadAssets();
  void LoadLightTexture();

  bool m_sceneInit{false};

  DrawablePool* m_mainPool;
  DrawableID m_sponzaId;

  SamplerDesc m_lightSamplerDesc{};

  UniformBufferData m_sceneUBO;

  ForwardRenderer m_forwardRenderer;

  Memory::HeapMemoryBuffer m_mainBuffer;

  Memory::RangeAllocator m_mainAllocator;
  Memory::RangeAllocator m_drawableAllocator;

  LightTexture m_lightTexture;
  Containers::Vector<PointLight> m_lights;

  FlyThroughCamera m_camera;

  std::unique_ptr<Renderer> m_renderer{nullptr};
  std::unique_ptr<Window> m_window{nullptr};
  std::unique_ptr<TextureManager> m_textureManager{nullptr};

  Log log_AppRenderer;
};
} // namespace