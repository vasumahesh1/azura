#pragma once
#include "App/Scene.h"
#include "Types.h"
#include "App/LightTexture.h"


namespace Azura {

struct ForwardComputePass {
  U32 m_passId;
  U32 m_computePassId;

  U32 m_uboSlot;

  U32 m_sampSlot;
  U32 m_lightSampSlot;

  U32 m_texSlot;
  U32 m_normalSlot;

  U32 m_computeUBO;
};


class ForwardComputeScene : public Scene {

public:
  explicit ForwardComputeScene(Memory::Allocator& mainAllocator, Memory::Allocator& drawAllocator);

  void Initialize(Window& window,
                  const MeshObject& sceneMesh,
                  const UniformBufferData& uboData,
                  const SamplerDesc& lightSamplerDesc,
                  const Containers::Vector<PointLight>& lights) override;
  void Update(float timeDelta, const UniformBufferData& uboData, const Containers::Vector<PointLight>& lights) override;
  void CleanUp() override;

private:
  ForwardComputePass m_pass{};
  DrawablePool* m_mainPool{nullptr};
  ComputePool* m_computePool{nullptr};
  LightTexture m_lightTexture;
};

} // namespace Azura
