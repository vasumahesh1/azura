#pragma once

#include "App/Common.h"
#include "Generic/Window.h"
#include "Generic/TextureManager.h"

namespace Azura {

class Scene {
public:
  Scene(const String& name, Memory::Allocator& mainAllocator, Memory::Allocator& drawAllocator)
    : log_Scene(name),
      m_mainAllocator(mainAllocator),
      m_drawAllocator(drawAllocator) {
  }

  virtual ~Scene() = default;

  Scene(const Scene& other) = delete;
  Scene(Scene&& other) noexcept = delete;
  Scene& operator=(const Scene& other) = delete;
  Scene& operator=(Scene&& other) noexcept = delete;

  virtual void Initialize(Window& window,
                          const MeshObject& sceneMesh,
                          const UniformBufferData& uboData,
                          const SamplerDesc& lightSamplerDesc,
                          const Containers::Vector<PointLight>& lights) = 0;
  virtual void Update(float timeDelta,
                      const UniformBufferData& uboData,
                      const Containers::Vector<PointLight>& lights) = 0;
  virtual void CleanUp() = 0;

protected:
  Log log_Scene;
  Memory::Allocator& m_mainAllocator;
  Memory::Allocator& m_drawAllocator;

  std::unique_ptr<Renderer> m_renderer{nullptr};
  std::unique_ptr<TextureManager> m_textureManager{nullptr};

  U32 m_sponzaId{0};
};

} // namespace Azura
