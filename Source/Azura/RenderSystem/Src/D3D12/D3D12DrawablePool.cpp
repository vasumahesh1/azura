#include "D3D12/D3D12DrawablePool.h"

#include "D3D12/d3dx12.h"
#include "D3D12/D3D12Macros.h"

using namespace Microsoft::WRL; // NOLINT

namespace Azura {
namespace D3D12 {

D3D12DrawablePool::D3D12DrawablePool(const ComPtr<ID3D12Device>& device,
                                     const DrawablePoolCreateInfo& createInfo,
                                     const DescriptorCount& descriptorCount,
                                     const Containers::Vector<DescriptorSlot>& descriptorSlots,
                                     const Containers::Vector<D3D12ScopedShader>& shaders,
                                     Memory::Allocator& mainAllocator,
                                     Memory::Allocator& initAllocator,
                                     Log log)
  : DrawablePool(createInfo, descriptorCount, mainAllocator),
    log_D3D12RenderSystem(std::move(log)),
    m_device(device),
    m_descriptorSlots(descriptorSlots),
    m_shaders(shaders),
    m_pipelines(mainAllocator),
    m_drawables(createInfo.m_numDrawables , mainAllocator),
    m_pipelineFactory(initAllocator, log_D3D12RenderSystem),
    m_secondaryCommandBuffer(device, D3D12_COMMAND_LIST_TYPE_BUNDLE, log_D3D12RenderSystem) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "Creating D3D12 Drawable Pool");
  CreateRootSignature(device);
  CreateInputAttributes(createInfo);
  CreateDescriptorHeap(createInfo);

  // Create Buffer
  m_stagingBuffer.Create(device, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), createInfo.m_byteSize, D3D12_RESOURCE_STATE_GENERIC_READ, log_D3D12RenderSystem);
}

DrawableID D3D12DrawablePool::CreateDrawable(const DrawableCreateInfo& createInfo) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "Creating a D3D12 Drawable");

  D3D12Drawable drawable = D3D12Drawable(createInfo, m_numVertexSlots, m_numInstanceSlots, m_descriptorCount.m_numUniformSlots, GetAllocator());
  m_drawables.PushBack(std::move(drawable));
  return m_drawables.GetSize() - 1;
}

void D3D12DrawablePool::BindVertexData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL,
    "D3D12 Drawable Pool: Binding Vertex Requested for Drawable: %d for Slot: %d of Size: %d bytes", drawableId, slot,
    size);

  assert(m_drawables.GetSize() > drawableId);

  auto& drawable = m_drawables[drawableId];

  // TODO(vasumahesh1):[INPUT]: Could be an issue with sizeof(float)
  const U32 offset = m_stagingBuffer.AppendData(buffer, size, sizeof(float), log_D3D12RenderSystem);

  BufferInfo info    = BufferInfo();
  info.m_maxByteSize = size;
  info.m_byteSize    = size;
  info.m_offset      = offset;
  info.m_binding     = slot;

  drawable.AddVertexBufferInfo(std::move(info));
}

void D3D12DrawablePool::BindInstanceData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL,
    "D3D12 Drawable Pool: Binding Instance Requested for Drawable: %d for Slot: %d of Size: %d bytes", drawableId, slot,
    size);

  assert(m_drawables.GetSize() > drawableId);

  auto& drawable = m_drawables[drawableId];

  // TODO(vasumahesh1):[INPUT]: Could be an issue with sizeof(float)
  const U32 offset = m_stagingBuffer.AppendData(buffer, size, sizeof(float), log_D3D12RenderSystem);

  BufferInfo info    = BufferInfo();
  info.m_maxByteSize = size;
  info.m_byteSize    = size;
  info.m_offset      = offset;
  info.m_binding     = slot;

  drawable.AddInstanceBufferInfo(std::move(info));
}

void D3D12DrawablePool::BindUniformData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL,
    "D3D12 Drawable Pool: Binding Uniform Requested for Drawable: %d for Slot: %d of Size: %d bytes", drawableId, slot,
    size);

  assert(m_drawables.GetSize() > drawableId);

  auto& drawable = m_drawables[drawableId];

  size = (size + D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);

  // TODO(vasumahesh1):[INPUT]: Could be an issue with sizeof(float)
  const U32 offset = m_stagingBuffer.AppendData(buffer, size, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT, log_D3D12RenderSystem);

  const auto& descriptorSlot = m_descriptorSlots[slot];

  UniformBufferInfo info = UniformBufferInfo();
  info.m_byteSize        = size;
  info.m_offset          = offset;
  info.m_binding         = descriptorSlot.m_bindIdx;
  info.m_set             = descriptorSlot.m_setIdx;

  drawable.AddUniformBufferInfo(std::move(info));
}

void D3D12DrawablePool::SetIndexData(DrawableID drawableId, const U8* buffer, U32 size) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL,
    "D3D12 Drawable Pool: Binding Index Requested for Drawable: %d of Size: %d bytes", drawableId, size);

  auto& drawable = m_drawables[drawableId];

  const U32 offset = m_stagingBuffer.AppendData(buffer, size, sizeof(U32), log_D3D12RenderSystem);

  BufferInfo info = BufferInfo();
  info.m_byteSize = size;
  info.m_offset   = offset;

  drawable.SetIndexBufferInfo(std::move(info));
}

void D3D12DrawablePool::AddShader(U32 shaderId) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Binding Shader Requested, ID: %d", shaderId);
  m_pipelineFactory.AddShaderStage(m_shaders[shaderId]);
}

void D3D12DrawablePool::BindTextureData(SlotID slot, const TextureDesc& desc, const U8* buffer) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL,
    "D3D12 Drawable Pool: Binding Texture Requested for Slot: %d of Size: %d bytes", slot, desc.m_size);

  UNUSED(buffer);
}

void D3D12DrawablePool::BindSampler(SlotID slot, const SamplerDesc& desc) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Binding Sampler Requested for Slot: %d", slot);
  UNUSED(desc);
}

void D3D12DrawablePool::Submit() {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Submitting");

  m_pipelineFactory.Submit(m_device, m_rootSignature, m_pipelines);

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Created Pipelines");

  m_secondaryCommandBuffer.CreateGraphicsCommandList(m_device, m_pipelines[0].GetState(), log_D3D12RenderSystem);
  auto bundleCommandList = m_secondaryCommandBuffer.GetGraphicsCommandList();

  const auto heapElementSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

  CD3DX12_CPU_DESCRIPTOR_HANDLE heapHandle(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart());

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Creating Resource Views");

  U32 idx = 0;
  for (auto& drawable : m_drawables) {
    const U32 heapOffset = idx * m_descriptorsPerDrawable * heapElementSize;
    heapHandle.Offset(heapOffset);

    drawable.CreateResourceViews(m_device, m_stagingBuffer.Real(), m_vertexDataSlots, heapHandle, heapElementSize, log_D3D12RenderSystem);
    ++idx;
  }

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Created Resource Views");

  ID3D12DescriptorHeap* ppHeaps[] = { m_descriptorHeap.Get() };
  bundleCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps); // NOLINT
  bundleCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  bundleCommandList->SetGraphicsRootSignature(m_rootSignature.Get());

  CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHeapHandle(m_descriptorHeap->GetGPUDescriptorHandleForHeapStart());

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Recording Commands");

  idx = 0;
  for (auto& drawable : m_drawables) {
    const U32 heapOffset = idx * m_descriptorsPerDrawable * heapElementSize;
    gpuHeapHandle.Offset(heapOffset);

    drawable.RecordCommands(bundleCommandList, gpuHeapHandle, heapElementSize, log_D3D12RenderSystem);
    ++idx;
  }

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Closing Bundle Command Buffer");
  VERIFY_D3D_OP(log_D3D12RenderSystem, bundleCommandList->Close(), "Failed to close bundle Command Buffer");
}

ID3D12DescriptorHeap* D3D12DrawablePool::GetDescriptorHeap() const {
  return m_descriptorHeap.Get();
}

ID3D12RootSignature* D3D12DrawablePool::GetRootSignature() const {
  return m_rootSignature.Get();
}
  
ID3D12PipelineState* D3D12DrawablePool::GetPipelineState() const {
  return m_pipelines[0].GetState();
}

ID3D12GraphicsCommandList* D3D12DrawablePool::GetSecondaryCommandList() const {
  return m_secondaryCommandBuffer.GetGraphicsCommandList();
}

void D3D12DrawablePool::CreateRootSignature(const ComPtr<ID3D12Device>& device) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Creating Root Signature");

  CD3DX12_DESCRIPTOR_RANGE range;
  CD3DX12_ROOT_PARAMETER parameter;

  range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
  parameter.InitAsDescriptorTable(1, &range, D3D12_SHADER_VISIBILITY_VERTEX);

  CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
  rootSignatureDesc.Init(1, &parameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

  ComPtr<ID3DBlob> signature;
  ComPtr<ID3DBlob> error;
  VERIFY_D3D_OP(log_D3D12RenderSystem, D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &
    signature, &error), "Failed to serialize D3D12 Root Signature");
  VERIFY_D3D_OP(log_D3D12RenderSystem, device->CreateRootSignature(0, signature->GetBufferPointer(), signature->
    GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)), "Failed to create Root Signature");
}

void D3D12DrawablePool::CreateInputAttributes(const DrawablePoolCreateInfo& createInfo) {
  U32 idx = 0;
  for (const auto& vertexSlot : createInfo.m_vertexDataSlots) {
    m_pipelineFactory.BulkAddAttributeDescription(vertexSlot, idx);
    ++idx;
  }
}

void D3D12DrawablePool::CreateDescriptorHeap(const DrawablePoolCreateInfo& createInfo) {
  m_descriptorsPerDrawable = m_descriptorCount.m_numUniformSlots + m_descriptorCount.m_numSampledImageSlots;

  D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
  heapDesc.NumDescriptors = createInfo.m_numDrawables * m_descriptorsPerDrawable;
  heapDesc.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
  heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
  VERIFY_D3D_OP(log_D3D12RenderSystem, m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_descriptorHeap)), "Failed to create CBV SRV UAV Descriptor Heap");
}
} // namespace D3D12
} // namespace Azura
