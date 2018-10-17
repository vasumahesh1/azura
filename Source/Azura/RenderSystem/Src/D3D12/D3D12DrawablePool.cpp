#include "D3D12/D3D12DrawablePool.h"


namespace Azura {
namespace D3D12 {

D3D12DrawablePool::D3D12DrawablePool(const DrawablePoolCreateInfo& createInfo,
                                     const DescriptorCount& descriptorCount,
                                     Memory::Allocator& allocator)
  : DrawablePool(createInfo, descriptorCount, allocator) {
}

DrawableID D3D12DrawablePool::CreateDrawable(const DrawableCreateInfo& createInfo) {
}

void D3D12DrawablePool::AddBufferBinding(SlotID slot, const Containers::Vector<RawStorageFormat>& strides) {
}

void D3D12DrawablePool::BindVertexData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) {
}

void D3D12DrawablePool::BindInstanceData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) {
}

void D3D12DrawablePool::BindUniformData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) {
}

void D3D12DrawablePool::SetIndexData(DrawableID drawableId, const U8* buffer, U32 size) {
}

void D3D12DrawablePool::AddShader(U32 shaderId) {
}

void D3D12DrawablePool::BindTextureData(SlotID slot, const TextureDesc& desc, const U8* buffer) {
}

void D3D12DrawablePool::BindSampler(SlotID slot, const SamplerDesc& desc) {
}

void D3D12DrawablePool::Submit() {
}
} // namespace D3D12
} // namespace Azura