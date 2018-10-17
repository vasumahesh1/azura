#pragma once
#include "Generic/Drawable.h"


namespace Azura {
namespace D3D12 {

class D3D12DrawablePool : public DrawablePool
{

public:
  D3D12DrawablePool(const DrawablePoolCreateInfo& createInfo,
                    const DescriptorCount& descriptorCount,
                    Memory::Allocator& allocator);

  DrawableID CreateDrawable(const DrawableCreateInfo& createInfo) override;
  void AddBufferBinding(SlotID slot, const Containers::Vector<RawStorageFormat>& strides) override;
  void BindVertexData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) override;
  void BindInstanceData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) override;
  void BindUniformData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) override;
  void SetIndexData(DrawableID drawableId, const U8* buffer, U32 size) override;
  void AddShader(U32 shaderId) override;
  void BindTextureData(SlotID slot, const TextureDesc& desc, const U8* buffer) override;
  void BindSampler(SlotID slot, const SamplerDesc& desc) override;
  void Submit() override;
};

} // namespace D3D12
} // namespace Azura