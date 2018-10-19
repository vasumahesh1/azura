#include "D3D12/D3D12DrawablePool.h"

#include "D3D12/d3dx12.h"
#include "D3D12/D3D12Macros.h"
#include "Memory/MemoryFactory.h"
#include "Memory/MonotonicAllocator.h"
#include "D3D12/D3D12ScopedImage.h"
#include <algorithm>

using namespace Microsoft::WRL;    // NOLINT
using namespace Azura::Containers; // NOLINT

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
    m_drawables(createInfo.m_numDrawables, mainAllocator),
    m_pipelineFactory(initAllocator, log_D3D12RenderSystem),
    m_descriptorsPerDrawable(descriptorCount.m_numUniformSlots),
    m_descriptorTableSizes(mainAllocator),
    m_images(mainAllocator),
    m_samplers(mainAllocator),
    m_secondaryCommandBuffer(device, D3D12_COMMAND_LIST_TYPE_BUNDLE, log_D3D12RenderSystem),
    m_allHeaps(mainAllocator) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "Creating D3D12 Drawable Pool");
  CreateRootSignature(device);
  CreateInputAttributes(createInfo);
  CreateDescriptorHeap(createInfo);

  // Create Buffer
  m_stagingBuffer.Create(device, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), createInfo.m_byteSize,
                         D3D12_RESOURCE_STATE_GENERIC_READ, log_D3D12RenderSystem);
  m_stagingBuffer.Map();
  m_mainBuffer.Create(device, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), createInfo.m_byteSize,
                      D3D12_RESOURCE_STATE_COPY_DEST, log_D3D12RenderSystem);
}

DrawableID D3D12DrawablePool::CreateDrawable(const DrawableCreateInfo& createInfo) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "Creating a D3D12 Drawable");

  D3D12Drawable drawable = D3D12Drawable(createInfo, m_numVertexSlots, m_numInstanceSlots,
                                         m_descriptorCount.m_numUniformSlots, GetAllocator());
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

  size = (size + D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1) & ~(
           D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);

  // TODO(vasumahesh1):[INPUT]: Could be an issue with sizeof(float)
  const U32 offset = m_stagingBuffer.AppendData(buffer, size, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT,
                                                log_D3D12RenderSystem);

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

  const U32 size = desc.m_size;

  const auto& descriptorSlot = m_descriptorSlots[slot];

  // TODO(vasumahesh1):[INPUT]: Could be an issue with sizeof(float)
  const U32 offset = m_stagingBuffer.AppendData(buffer, size, sizeof(float), log_D3D12RenderSystem);

  TextureBufferInfo info = TextureBufferInfo();
  info.m_byteSize        = size;
  info.m_offset          = offset;
  info.m_desc            = desc;
  info.m_binding         = descriptorSlot.m_bindIdx;
  info.m_set             = descriptorSlot.m_setIdx;

  m_textureBufferInfos.PushBack(info);
}

void D3D12DrawablePool::BindSampler(SlotID slot, const SamplerDesc& desc) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Binding Sampler Requested for Slot: %d", slot);
  UNUSED(desc);

  const auto& descriptorSlot = m_descriptorSlots[slot];
  assert(descriptorSlot.m_type == DescriptorType::Sampler);

  SamplerInfo sInfo = {};
  sInfo.m_set       = descriptorSlot.m_setIdx;
  sInfo.m_binding   = descriptorSlot.m_bindIdx;

  m_samplerInfos.PushBack(sInfo);
}

void D3D12DrawablePool::SetTextureData(ID3D12GraphicsCommandList* oneTimeCommandList) {
  if (m_textureBufferInfos.GetSize() == 0) {
    return;
  }

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Texture Data Found");

  const CD3DX12_CPU_DESCRIPTOR_HANDLE textureCPUHandle(m_descriptorDrawableHeap->GetCPUDescriptorHandleForHeapStart());

  m_images.Reserve(m_textureBufferInfos.GetSize());

  U32 idx = 0;
  for (const auto& textBufInfo : m_textureBufferInfos) {

    D3D12ScopedImage image = {};
    image.Create(m_device, D3D12_RESOURCE_STATE_COPY_DEST, textBufInfo.m_desc, log_D3D12RenderSystem);
    image.CopyFromBuffer(m_device, oneTimeCommandList, m_mainBuffer, textBufInfo.m_offset);
    image.Transition(oneTimeCommandList, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    m_images.PushBack(std::move(image));

    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    CD3DX12_CPU_DESCRIPTOR_HANDLE::InitOffsetted(cpuHandle, textureCPUHandle, m_cbvSrvDescriptorElementSize * idx);

    const auto srvDesc = D3D12ScopedImage::
      GetSRV(textBufInfo.m_desc, ImageViewType::ImageView2D, log_D3D12RenderSystem);
    m_device->CreateShaderResourceView(m_images.Last().Real(), &srvDesc, cpuHandle);
    ++idx;
  }

  const CD3DX12_CPU_DESCRIPTOR_HANDLE samplerCPUHandle(m_descriptorSamplerHeap->GetCPUDescriptorHandleForHeapStart());

  m_samplers.Reserve(m_samplerInfos.GetSize());

  idx = 0;
  for (const auto& samplerInfo : m_samplerInfos) {
    D3D12ScopedSampler sampler = {};
    sampler.Create();

    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    CD3DX12_CPU_DESCRIPTOR_HANDLE::InitOffsetted(cpuHandle, samplerCPUHandle, m_cbvSrvDescriptorElementSize * idx);

    m_device->CreateSampler(&sampler.GetDesc(), cpuHandle);

    ++idx;
  }
}

void D3D12DrawablePool::RecordTextureData(ID3D12GraphicsCommandList* bundleCommandList) {
  if (m_textureBufferInfos.GetSize() == 0) {
    return;
  }

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Recording Texture Data");

  const CD3DX12_GPU_DESCRIPTOR_HANDLE textureGPUHandle(m_descriptorDrawableHeap->GetGPUDescriptorHandleForHeapStart());

  U32 idx = 0;
  int lastSet = -1;
  for (const auto& textBufInfo : m_textureBufferInfos) {
    if (int(textBufInfo.m_set) == lastSet)
    {
      ++idx;
      continue;
    }

    lastSet = textBufInfo.m_set;

    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle;
    CD3DX12_GPU_DESCRIPTOR_HANDLE::InitOffsetted(gpuHandle, textureGPUHandle, m_cbvSrvDescriptorElementSize * idx);

    bundleCommandList->SetGraphicsRootDescriptorTable(textBufInfo.m_set, gpuHandle);
    ++idx;
  }

  const CD3DX12_GPU_DESCRIPTOR_HANDLE samplerGPUHandle(m_descriptorSamplerHeap->GetGPUDescriptorHandleForHeapStart());


  lastSet = -1;
  idx = 0;
  for (const auto& samplerInfo : m_samplerInfos) {
    if (int(samplerInfo.m_set) == lastSet)
    {
      ++idx;
      continue;
    }

    lastSet = samplerInfo.m_set;

    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle;
    CD3DX12_GPU_DESCRIPTOR_HANDLE::InitOffsetted(gpuHandle, samplerGPUHandle, m_cbvSrvDescriptorElementSize * idx);

    bundleCommandList->SetGraphicsRootDescriptorTable(samplerInfo.m_set, gpuHandle);
    ++idx;
  }
}

void D3D12DrawablePool::Submit() {
  std::sort(m_textureBufferInfos.Begin(), m_textureBufferInfos.End(), [](const TextureBufferInfo& a, const TextureBufferInfo& b) -> bool
  {
    if (a.m_set == b.m_set)
    {
      return a.m_binding < b.m_binding;
    }

    return a.m_set < b.m_set;
  });

  std::sort(m_samplerInfos.Begin(), m_samplerInfos.End(), [](const SamplerInfo& a, const SamplerInfo& b) -> bool
  {
    if (a.m_set == b.m_set)
    {
      return a.m_binding < b.m_binding;
    }

    return a.m_set < b.m_set;
  });

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Submitting");
  m_pipelineFactory.Submit(m_device, m_rootSignature, m_pipelines);

  // Create a Command Buffer to submit data for drawable
  // Describe and create the command queue.
  D3D12_COMMAND_QUEUE_DESC queueDesc = {};
  queueDesc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
  queueDesc.Type                     = D3D12_COMMAND_LIST_TYPE_DIRECT;

  ComPtr<ID3D12CommandQueue> commandQueue;
  VERIFY_D3D_OP(log_D3D12RenderSystem, m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)),
    "Failed to create command Queue");

  auto oneTimeSubmitBuffer = D3D12ScopedCommandBuffer(m_device, D3D12_COMMAND_LIST_TYPE_DIRECT, log_D3D12RenderSystem);
  oneTimeSubmitBuffer.CreateGraphicsCommandList(m_device, m_pipelines[0].GetState(), log_D3D12RenderSystem);

  auto oneTimeCommandList = oneTimeSubmitBuffer.GetGraphicsCommandList();

  const UINT64 stagingBufferSize = GetRequiredIntermediateSize(m_stagingBuffer.Real(), 0, 1);
  D3D12Core::CopyBuffer(oneTimeCommandList, m_mainBuffer, m_stagingBuffer, stagingBufferSize);

  m_mainBuffer.Transition(oneTimeCommandList, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COPY_SOURCE);

  SetTextureData(oneTimeCommandList);

  m_mainBuffer.Transition(oneTimeCommandList, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

  oneTimeCommandList->Close();

  oneTimeSubmitBuffer.Execute(m_device, commandQueue.Get(), log_D3D12RenderSystem);
  oneTimeSubmitBuffer.WaitForComplete(commandQueue.Get(), log_D3D12RenderSystem);

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Created Pipelines");

  m_cbvSrvDescriptorElementSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
  const U32 heapStepOffset      = m_descriptorsPerDrawable * m_cbvSrvDescriptorElementSize;

  CD3DX12_CPU_DESCRIPTOR_HANDLE heapHandle(m_descriptorDrawableHeap->GetCPUDescriptorHandleForHeapStart(), m_offsetToDrawableHeap, m_cbvSrvDescriptorElementSize);

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Creating Resource Views");

  for (auto& drawable : m_drawables) {
    drawable.CreateResourceViews(m_device, m_mainBuffer.Real(), m_vertexDataSlots, heapHandle,
                                 m_cbvSrvDescriptorElementSize, m_descriptorTableSizes, log_D3D12RenderSystem);
    heapHandle.Offset(heapStepOffset);
  }

  // Record Command Buffer

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Begin Recording");

  m_secondaryCommandBuffer.CreateGraphicsCommandList(m_device, m_pipelines[0].GetState(), log_D3D12RenderSystem);
  auto bundleCommandList = m_secondaryCommandBuffer.GetGraphicsCommandList();

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Created Resource Views");

  bundleCommandList->SetDescriptorHeaps(UINT(m_allHeaps.GetSize()), m_allHeaps.Data());
  bundleCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  bundleCommandList->SetGraphicsRootSignature(m_rootSignature.Get());

  RecordTextureData(bundleCommandList);

  CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHeapHandle(m_descriptorDrawableHeap->GetGPUDescriptorHandleForHeapStart(), m_offsetToDrawableHeap, m_cbvSrvDescriptorElementSize);
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Recording Commands");

  for (auto& drawable : m_drawables) {
    drawable.RecordCommands(bundleCommandList, gpuHeapHandle, m_cbvSrvDescriptorElementSize, m_descriptorTableSizes,
                            log_D3D12RenderSystem);
    gpuHeapHandle.Offset(heapStepOffset);
  }

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Closing Bundle Command Buffer");
  VERIFY_D3D_OP(log_D3D12RenderSystem, bundleCommandList->Close(), "Failed to close bundle Command Buffer");
}

const Vector<ID3D12DescriptorHeap*>& D3D12DrawablePool::GetAllDescriptorHeaps() const {
  return m_allHeaps;
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
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 2048);

  m_descriptorTableSizes.Reserve(m_descriptorSlots.GetSize());

  int totalCount = 0;
  for (const auto& slot : m_descriptorSlots) {
    if (slot.m_binding == DescriptorBinding::Same) {
      m_descriptorTableSizes.Last().m_count += 1;

      ++totalCount;
      continue;
    }

    m_descriptorTableSizes.PushBack({1, totalCount});
    ++totalCount;
  }

  Vector<CD3DX12_ROOT_PARAMETER> descriptorTables(m_descriptorTableSizes.GetSize(), allocatorTemporary);

  int offset = 0;
  for (const auto& bindingSize : m_descriptorTableSizes) {
    Vector<CD3DX12_DESCRIPTOR_RANGE> currentRanges(bindingSize.m_count, allocatorTemporary);

    U32 cbvOffset     = 0;
    U32 srvOffset     = 0;
    U32 samplerOffset = 0;

    for (int idx = offset; idx < offset + bindingSize.m_count; ++idx) {
      CD3DX12_DESCRIPTOR_RANGE rangeData;

      const auto& slot = m_descriptorSlots[idx];

      switch (slot.m_type) {
        case DescriptorType::UniformBuffer:
          rangeData.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, cbvOffset);
          currentRanges.PushBack(rangeData);
          ++cbvOffset;
          break;

        case DescriptorType::Sampler:
          rangeData.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, samplerOffset);
          currentRanges.PushBack(rangeData);
          ++samplerOffset;
          break;

        case DescriptorType::SampledImage:
          rangeData.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, srvOffset);
          currentRanges.PushBack(rangeData);
          ++srvOffset;
          break;

        case DescriptorType::PushConstant:
        case DescriptorType::CombinedImageSampler:
        default:
          LOG_ERR(log_D3D12RenderSystem, LOG_LEVEL, "Unsupported Descriptor Type");
          break;
      }

      CD3DX12_ROOT_PARAMETER rootParameter;
      rootParameter.InitAsDescriptorTable(currentRanges.GetSize(), currentRanges.Data(), D3D12_SHADER_VISIBILITY_ALL);
      descriptorTables.PushBack(rootParameter);
    }

    offset += bindingSize.m_count;
  }

  CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
  rootSignatureDesc.Init(descriptorTables.GetSize(), descriptorTables.Data(), 0, nullptr,
                         D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

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
  m_allHeaps.Reserve(3);

  m_offsetToDrawableHeap = m_descriptorCount.m_numSampledImageSlots;

  D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
  heapDesc.NumDescriptors             = m_offsetToDrawableHeap + createInfo.m_numDrawables * m_descriptorsPerDrawable;
  heapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
  heapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
  VERIFY_D3D_OP(log_D3D12RenderSystem, m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_descriptorDrawableHeap)
  ), "Failed to create Drawable Descriptor Heap");
  m_allHeaps.PushBack(m_descriptorDrawableHeap.Get());

  if (m_descriptorCount.m_numSamplerSlots > 0) {
    D3D12_DESCRIPTOR_HEAP_DESC samplerDesc = {};
    samplerDesc.NumDescriptors             = m_descriptorCount.m_numSamplerSlots;
    samplerDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    samplerDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    VERIFY_D3D_OP(log_D3D12RenderSystem, m_device->CreateDescriptorHeap(&samplerDesc, IID_PPV_ARGS(&
      m_descriptorSamplerHeap)), "Failed to create Sampler Descriptor Heap");

    m_allHeaps.PushBack(m_descriptorSamplerHeap.Get());
  }
}
} // namespace D3D12
} // namespace Azura
