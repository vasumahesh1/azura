#include "D3D12/D3D12DrawablePool.h"

#include "D3D12/d3dx12.h"
#include "D3D12/D3D12Macros.h"

using namespace Microsoft::WRL; // NOLINT

namespace Azura {
namespace D3D12 {

D3D12DrawablePool::D3D12DrawablePool(const ComPtr<ID3D12Device>& device,
                                     const DrawablePoolCreateInfo& createInfo,
                                     const DescriptorCount& descriptorCount,
                                     const Containers::Vector<D3D12ScopedShader>& shaders,
                                     Memory::Allocator& mainAllocator,
                                     Memory::Allocator& initAllocator,
                                     Log log)
  : DrawablePool(createInfo, descriptorCount, mainAllocator),
    log_D3D12RenderSystem(std::move(log)),
    m_device(device),
    m_shaders(shaders),
    m_pipelines(mainAllocator),
    m_pipelineFactory(initAllocator, log_D3D12RenderSystem) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "Creating D3D12 Drawable Pool");
  CreateRootSignature(device);
  CreateInputAttributes(createInfo);
}

DrawableID D3D12DrawablePool::CreateDrawable(const DrawableCreateInfo& createInfo) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "Creating a D3D12 Drawable");
}

void D3D12DrawablePool::BindVertexData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Binding Vertex Requested for Drawable: %d for Slot: %d of Size: %d bytes", drawableId, slot, size);
}

void D3D12DrawablePool::BindInstanceData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Binding Instance Requested for Drawable: %d for Slot: %d of Size: %d bytes", drawableId, slot, size);
}

void D3D12DrawablePool::BindUniformData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Binding Uniform Requested for Drawable: %d for Slot: %d of Size: %d bytes", drawableId, slot, size);
}

void D3D12DrawablePool::SetIndexData(DrawableID drawableId, const U8* buffer, U32 size) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Binding Index Requested for Drawable: %d of Size: %d bytes", drawableId, size);
}

void D3D12DrawablePool::AddShader(U32 shaderId) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Binding Shader Requested, ID: %d", shaderId);
  m_pipelineFactory.AddShaderStage(m_shaders[shaderId]);
}

void D3D12DrawablePool::BindTextureData(SlotID slot, const TextureDesc& desc, const U8* buffer) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Binding Texture Requested for Slot: %d of Size: %d bytes", slot, desc.m_size);
}

void D3D12DrawablePool::BindSampler(SlotID slot, const SamplerDesc& desc) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Binding Sampler Requested for Slot: %d", slot);
}

void D3D12DrawablePool::Submit() {
  m_pipelineFactory.Submit(m_device, m_rootSignature, m_pipelines);
}

void D3D12DrawablePool::CreateRootSignature(const ComPtr<ID3D12Device>& device) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Creating Root Signature");
  CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
  rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

  ComPtr<ID3DBlob> signature;
  ComPtr<ID3DBlob> error;
  VERIFY_D3D_OP(log_D3D12RenderSystem, D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error), "Failed to serialize D3D12 Root Signature");
  VERIFY_D3D_OP(log_D3D12RenderSystem, device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)), "Failed to create Root Signature");
}

void D3D12DrawablePool::CreateInputAttributes(const DrawablePoolCreateInfo& createInfo) {
  U32 idx = 0;
  for(const auto& vertexSlot : createInfo.m_vertexDataSlots)
  {
    m_pipelineFactory.BulkAddAttributeDescription(vertexSlot, idx);
    ++idx;
  }
}
} // namespace D3D12
} // namespace Azura
