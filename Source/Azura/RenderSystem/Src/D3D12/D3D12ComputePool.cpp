#include "D3D12/D3D12ComputePool.h"

#include "D3D12/d3dx12.h"
#include "D3D12/D3D12Macros.h"
#include "Memory/MemoryFactory.h"
#include "Memory/MonotonicAllocator.h"
#include "D3D12/D3D12ScopedImage.h"
#include <algorithm>
#include <utility>
#include "D3D12/D3D12ScopedCommandBuffer.h"
#include "D3D12/D3D12ScopedBuffer.h"

using namespace Microsoft::WRL;    // NOLINT
using namespace Azura::Containers; // NOLINT

namespace Azura {
namespace D3D12 {

D3D12ComputePool::D3D12ComputePool(const ComPtr<ID3D12Device>& device,
                                     const ComputePoolCreateInfo& createInfo,
                                     const DescriptorCount& descriptorCount,
                                     const Vector<DescriptorSlot>& descriptorSlots,
                                     const Vector<D3D12ScopedShader>& shaders,
                                     const Vector<D3D12ScopedComputePass>& renderPasses,
                                     ComPtr<ID3D12CommandQueue> commandQueue,
                                     ComPtr<ID3D12CommandQueue> graphicsQueue,
                                     Memory::Allocator& mainAllocator,
                                     Memory::Allocator& initAllocator,
                                     Log log)
  : ComputePool(createInfo, descriptorCount, mainAllocator),
    log_D3D12RenderSystem(std::move(log)),
    m_device(device),
    m_globalDescriptorSlots(descriptorSlots),
    m_shaders(shaders),
    m_pipelines(mainAllocator),
    m_computePasses(createInfo.m_computePasses.GetSize(), mainAllocator),
    m_computeCommandQueue(std::move(commandQueue)),
    m_graphicsCommandQueue(std::move(graphicsQueue)),
    m_pipelineFactory(initAllocator, log_D3D12RenderSystem),
    m_images(mainAllocator),
    m_samplers(mainAllocator),
    m_allHeaps(mainAllocator) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "Creating D3D12 Drawable Pool");

  m_pipelineFactory.SetPipelineType(PipelineType::Compute);

  CreateRenderPassReferences(createInfo, renderPasses);
  CreateDescriptorHeap();

  // Create Buffers
  m_stagingBuffer.Create(device, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), createInfo.m_byteSize,
                         D3D12_RESOURCE_STATE_GENERIC_READ, log_D3D12RenderSystem);
  m_stagingBuffer.Map();

  m_updateBuffer.Create(device, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), createInfo.m_byteSize,
    D3D12_RESOURCE_STATE_GENERIC_READ, log_D3D12RenderSystem);
  m_updateBuffer.Map();

  m_mainBuffer.Create(device, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), createInfo.m_byteSize,
                      D3D12_RESOURCE_STATE_COPY_DEST, log_D3D12RenderSystem);
}

void D3D12ComputePool::BindUniformData(SlotID slot, const U8* buffer, U32 size) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL,
    "D3D12 Drawable Pool: Binding Uniform Requested for Slot: %d of Size: %d bytes", slot,
    size);

  size = (size + D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1) & ~(
           D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);

  // TODO(vasumahesh1):[INPUT]: Could be an issue with sizeof(float)
  const U32 offset = m_stagingBuffer.AppendData(buffer, size, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT,
                                                log_D3D12RenderSystem);

  const auto& descriptorSlot = m_globalDescriptorSlots[slot];

  UniformBufferInfo info = UniformBufferInfo();
  info.m_byteSize        = size;
  info.m_offset          = offset;
  info.m_binding         = descriptorSlot.m_bindIdx;
  info.m_set             = descriptorSlot.m_setIdx;

  m_uniformBufferInfos.PushBack(std::move(info));
}

void D3D12ComputePool::AddShader(U32 shaderId) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Binding Shader Requested, ID: %d", shaderId);
  m_pipelineFactory.AddShaderStage(m_shaders[shaderId]);
}

void D3D12ComputePool::BindTextureData(SlotID slot, const TextureDesc& desc, const U8* buffer) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL,
    "D3D12 Drawable Pool: Binding Texture Requested for Slot: %d of Size: %d bytes", slot, desc.m_size);

  const U32 size = desc.m_size;

  const auto& descriptorSlot = m_globalDescriptorSlots[slot];

  const U32 textureWidthBytes = desc.m_bounds.m_width * GetFormatSize(desc.m_format);

  // TODO(vasumahesh1):[INPUT]: Could be an issue with sizeof(float)
  const U32 offset = m_stagingBuffer.AppendTextureData(buffer, size, 512, textureWidthBytes, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT, log_D3D12RenderSystem);

  TextureBufferInfo info = TextureBufferInfo();
  info.m_byteSize        = size;
  info.m_offset          = offset;
  info.m_desc            = desc;
  info.m_binding         = descriptorSlot.m_bindIdx;
  info.m_set             = descriptorSlot.m_setIdx;

  m_textureBufferInfos.PushBack(info);
}

void D3D12ComputePool::BindSampler(SlotID slot, const SamplerDesc& desc) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Binding Sampler Requested for Slot: %d", slot);

  const auto& descriptorSlot = m_globalDescriptorSlots[slot];
  assert(descriptorSlot.m_type == DescriptorType::Sampler);

  SamplerInfo sInfo = {};
  sInfo.m_set       = descriptorSlot.m_setIdx;
  sInfo.m_binding   = descriptorSlot.m_bindIdx;
  sInfo.m_desc      = desc;

  m_samplerInfos.PushBack(sInfo);
}

void D3D12ComputePool::SetUniformBufferData() {
  if (m_uniformBufferInfos.GetSize() == 0)
  {
    return;
  }

  const auto gpuAddress = m_mainBuffer.Real()->GetGPUVirtualAddress();

  const CD3DX12_CPU_DESCRIPTOR_HANDLE uboCPUHandle(m_descriptorComputeHeap->GetCPUDescriptorHandleForHeapStart(), m_offsetToConstantBuffers, m_cbvSrvDescriptorElementSize);

  U32 idx = 0;
  for (const auto& ubInfo : m_uniformBufferInfos) {
    CD3DX12_CPU_DESCRIPTOR_HANDLE handle;
    CD3DX12_CPU_DESCRIPTOR_HANDLE::InitOffsetted(handle, uboCPUHandle, m_cbvSrvDescriptorElementSize *  idx);

    D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc = {
      gpuAddress + ubInfo.m_offset, ubInfo.m_byteSize
    };

    m_device->CreateConstantBufferView(&constantBufferViewDesc, handle);
    ++idx;
  }
}

void D3D12ComputePool::SetTextureData(ID3D12GraphicsCommandList* oneTimeCommandList) {
  if (m_textureBufferInfos.GetSize() == 0) {
    return;
  }

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Texture Data Found");

  const CD3DX12_CPU_DESCRIPTOR_HANDLE textureCPUHandle(m_descriptorComputeHeap->GetCPUDescriptorHandleForHeapStart());

  m_images.Reserve(m_textureBufferInfos.GetSize());

  U32 idx = 0;
  for (const auto& textBufInfo : m_textureBufferInfos) {

    D3D12ScopedImage image = {};
    image.Create(m_device, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_FLAG_NONE, textBufInfo.m_desc,
                 log_D3D12RenderSystem);
    image.CopyFromBuffer(m_device, oneTimeCommandList, m_mainBuffer, textBufInfo.m_offset);
    image.Transition(oneTimeCommandList, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, log_D3D12RenderSystem);

    m_images.PushBack(std::move(image));

    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    CD3DX12_CPU_DESCRIPTOR_HANDLE::InitOffsetted(cpuHandle, textureCPUHandle, m_cbvSrvDescriptorElementSize * idx);

    const auto srvDesc = D3D12ScopedImage::
      GetSRV(textBufInfo.m_desc.m_format, ImageViewType::ImageView2D, log_D3D12RenderSystem);
    m_device->CreateShaderResourceView(m_images.Last().Real(), &srvDesc, cpuHandle);
    ++idx;
  }
}

void D3D12ComputePool::SetSamplerData() {
  if (m_samplerInfos.GetSize() == 0) {
    return;
  }

  const CD3DX12_CPU_DESCRIPTOR_HANDLE samplerCPUHandle(m_descriptorSamplerHeap->GetCPUDescriptorHandleForHeapStart());

  m_samplers.Reserve(m_samplerInfos.GetSize());

  U32 idx = 0;
  for (const auto& samplerInfo : m_samplerInfos) {
    D3D12ScopedSampler sampler = {};
    sampler.Create(samplerInfo.m_desc, log_D3D12RenderSystem);

    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    CD3DX12_CPU_DESCRIPTOR_HANDLE::InitOffsetted(cpuHandle, samplerCPUHandle, m_samplerDescriptorElementSize * idx);

    m_device->CreateSampler(&sampler.GetDesc(), cpuHandle);

    ++idx;
  }
}

void D3D12ComputePool::CreateComputePassInputTargetSRV(
  const Vector<std::reference_wrapper<D3D12ScopedImage>>& renderPassInputs,
  U32 offsetTillThis) const {
  const CD3DX12_CPU_DESCRIPTOR_HANDLE inputCPUHandle(m_descriptorComputeHeap->GetCPUDescriptorHandleForHeapStart(),
                                                     m_offsetToComputePassInputTargets + offsetTillThis,
                                                     m_cbvSrvDescriptorElementSize);

  U32 idx = 0;
  for (const auto& imageRef : renderPassInputs) {
    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    CD3DX12_CPU_DESCRIPTOR_HANDLE::InitOffsetted(cpuHandle, inputCPUHandle, m_cbvSrvDescriptorElementSize * idx);

    const auto srvDesc = D3D12ScopedImage::GetSRV(imageRef.get().GetFormat(), ImageViewType::ImageView2D,
                                                  log_D3D12RenderSystem);
    m_device->CreateShaderResourceView(imageRef.get().Real(), &srvDesc, cpuHandle);
    ++idx;
  }
}

void D3D12ComputePool::CreateComputePassInputBufferUAV(
  const Vector<std::reference_wrapper<D3D12ScopedBuffer>>& bufferInputs,
  U32 offsetTillThis) const {
  const CD3DX12_CPU_DESCRIPTOR_HANDLE inputCPUHandle(m_descriptorComputeHeap->GetCPUDescriptorHandleForHeapStart(),
    m_offsetToComputePassInputBuffers + offsetTillThis,
    m_cbvSrvDescriptorElementSize);

  U32 idx = 0;
  for (const auto& bufferRef : bufferInputs) {
    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    CD3DX12_CPU_DESCRIPTOR_HANDLE::InitOffsetted(cpuHandle, inputCPUHandle, m_cbvSrvDescriptorElementSize * idx);

    const auto uavDesc = bufferRef.get().GetUAV();
    m_device->CreateUnorderedAccessView(bufferRef.get().Real(), nullptr, &uavDesc, cpuHandle);
    ++idx;
  }
}

void D3D12ComputePool::CreateComputePassInputTargetUAV(
  const Vector<std::reference_wrapper<D3D12ScopedImage>>& computePassOutputs,
  U32 offsetTillThis) const {
  const CD3DX12_CPU_DESCRIPTOR_HANDLE inputCPUHandle(m_descriptorComputeHeap->GetCPUDescriptorHandleForHeapStart(),
    m_offsetToComputePassOutputs + offsetTillThis,
    m_cbvSrvDescriptorElementSize);

  U32 idx = 0;
  for (const auto& imageRef : computePassOutputs) {
    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    CD3DX12_CPU_DESCRIPTOR_HANDLE::InitOffsetted(cpuHandle, inputCPUHandle, m_cbvSrvDescriptorElementSize * idx);

    const auto uavDesc = D3D12ScopedImage::GetUAV(imageRef.get().GetFormat(), ImageViewType::ImageView2D,
      log_D3D12RenderSystem);
    m_device->CreateUnorderedAccessView(imageRef.get().Real(), nullptr, &uavDesc, cpuHandle);
    ++idx;
  }
}

void D3D12ComputePool::Submit() {
  m_cbvSrvDescriptorElementSize  = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
  m_samplerDescriptorElementSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

  std::sort(m_textureBufferInfos.Begin(), m_textureBufferInfos.End(),
            [](const TextureBufferInfo& a, const TextureBufferInfo& b) -> bool
            {
              if (a.m_set == b.m_set) {
                return a.m_binding < b.m_binding;
              }

              return a.m_set < b.m_set;
            });

  std::sort(m_samplerInfos.Begin(), m_samplerInfos.End(), [](const SamplerInfo& a, const SamplerInfo& b) -> bool
  {
    if (a.m_set == b.m_set) {
      return a.m_binding < b.m_binding;
    }

    return a.m_set < b.m_set;
  });

  std::sort(m_uniformBufferInfos.Begin(), m_uniformBufferInfos.End(), [](const UniformBufferInfo& a, const UniformBufferInfo& b) -> bool
  {
    if (a.m_set == b.m_set) {
      return a.m_binding < b.m_binding;
    }

    return a.m_set < b.m_set;
  });

  U32 inputTargetsTillNow = 0;
  U32 inputBuffersTillNow = 0;
  U32 outputsTillNow = 0;
  for (U32 idx      = 0; idx < m_computePasses.GetSize(); ++idx) {
    const auto& renderPassInputTargets = m_computePasses[idx].get().GetInputImages();
    const auto& renderPassInputBuffers = m_computePasses[idx].get().GetInputBuffers();
    const auto& renderPassOutputs = m_computePasses[idx].get().GetOutputImages();

    if (renderPassInputTargets.GetSize() > 0) {
      CreateComputePassInputTargetSRV(renderPassInputTargets, inputTargetsTillNow);
      inputTargetsTillNow += renderPassInputTargets.GetSize();
    }
    
    if (renderPassInputBuffers.GetSize() > 0) {
      CreateComputePassInputBufferUAV(renderPassInputBuffers, inputBuffersTillNow);
      inputBuffersTillNow += renderPassInputBuffers.GetSize();
    }
    
    if (renderPassOutputs.GetSize() > 0) {
      CreateComputePassInputTargetUAV(renderPassOutputs, outputsTillNow);
      outputsTillNow += renderPassOutputs.GetSize();
    }
  }

  m_pipelines.Reserve(m_computePasses.GetSize());

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Submitting");
  m_pipelineFactory.Submit(m_device, m_computePasses, m_pipelines);

  auto oneTimeSubmitBuffer = D3D12ScopedCommandBuffer(m_device, D3D12_COMMAND_LIST_TYPE_DIRECT, log_D3D12RenderSystem);
  oneTimeSubmitBuffer.CreateGraphicsCommandList(m_device, nullptr, log_D3D12RenderSystem);

  auto oneTimeCommandList = oneTimeSubmitBuffer.GetGraphicsCommandList();

  const UINT64 stagingBufferSize = GetRequiredIntermediateSize(m_stagingBuffer.Real(), 0, 1);
  D3D12Core::CopyBuffer(oneTimeCommandList, m_mainBuffer, m_stagingBuffer, stagingBufferSize);

  m_mainBuffer.Transition(oneTimeCommandList, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COPY_SOURCE);

  SetTextureData(oneTimeCommandList);
  SetSamplerData();

  m_mainBuffer.Transition(oneTimeCommandList, D3D12_RESOURCE_STATE_COPY_SOURCE,
                          D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

  oneTimeCommandList->Close();

  oneTimeSubmitBuffer.Execute(m_device, m_graphicsCommandQueue.Get(), log_D3D12RenderSystem);
  oneTimeSubmitBuffer.WaitForComplete(m_graphicsCommandQueue.Get(), log_D3D12RenderSystem);

  SetUniformBufferData();

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Created Pipelines");

  
}

void D3D12ComputePool::Record()
{
  // Record Command Buffer

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Begin Recording");

  U32 idx = 0;

  U32 cbRecorded = 0;
  U32 sampledImageRecorded = 0;
  U32 inputTargetsRecorded       = 0;
  U32 inputBuffersRecorded       = 0;
  U32 outputsRecorded       = 0;
  U32 samplersRecorded     = 0;

  for (const auto& renderPassRef : m_computePasses) {
    const auto& renderPass                   = renderPassRef.get();

    ID3D12GraphicsCommandList* secondaryCommandList = renderPass.GetPrimaryComputeCommandList(0);

    const auto& renderPassDescriptorCount    = renderPass.GetDescriptorCount();
    const auto& renderPassRootSignatureTable = renderPass.GetRootSignatureTable();
    const auto& renderPassInputTargets             = renderPass.GetInputImages();
    const auto& renderPassInputBuffers             = renderPass.GetInputBuffers();
    const auto& renderPassOutputs             = renderPass.GetOutputImages();

    // Define Heap Handles
    CD3DX12_GPU_DESCRIPTOR_HANDLE textureGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_descriptorComputeHeap->GetGPUDescriptorHandleForHeapStart(),
      sampledImageRecorded, m_cbvSrvDescriptorElementSize);

    const CD3DX12_GPU_DESCRIPTOR_HANDLE inputTargetsGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_descriptorComputeHeap->GetGPUDescriptorHandleForHeapStart(),
      m_offsetToComputePassInputTargets + inputTargetsRecorded,
      m_cbvSrvDescriptorElementSize);
    
    const CD3DX12_GPU_DESCRIPTOR_HANDLE inputBuffersGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_descriptorComputeHeap->GetGPUDescriptorHandleForHeapStart(),
      m_offsetToComputePassInputBuffers + inputBuffersRecorded,
      m_cbvSrvDescriptorElementSize);

    const CD3DX12_GPU_DESCRIPTOR_HANDLE outputsGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_descriptorComputeHeap->GetGPUDescriptorHandleForHeapStart(),
      m_offsetToComputePassOutputs + outputsRecorded,
      m_cbvSrvDescriptorElementSize);

    CD3DX12_GPU_DESCRIPTOR_HANDLE samplerGPUHandle = {};
    if (renderPassDescriptorCount.m_numSamplerSlots > 0) {
      samplerGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_descriptorSamplerHeap->GetGPUDescriptorHandleForHeapStart(),
        samplersRecorded, m_samplerDescriptorElementSize);
    }

    CD3DX12_GPU_DESCRIPTOR_HANDLE uboGPUHandle(m_descriptorComputeHeap->GetGPUDescriptorHandleForHeapStart(), cbRecorded + m_offsetToConstantBuffers, m_cbvSrvDescriptorElementSize);

    secondaryCommandList->SetDescriptorHeaps(UINT(m_allHeaps.GetSize()), m_allHeaps.Data());
    secondaryCommandList->SetComputeRootSignature(renderPass.GetRootSignature());
    secondaryCommandList->SetPipelineState(m_pipelines[idx].GetState());

    U32 tableIdx = 0;
    for (const auto& tableEntry : renderPassRootSignatureTable) {
      if (tableEntry.m_type == DescriptorType::SampledImage) {
        LOG_DEBUG(log_D3D12RenderSystem, LOG_LEVEL, "Setting Texture Descriptor Table at %d", tableIdx);
        secondaryCommandList->SetComputeRootDescriptorTable(tableIdx, textureGPUHandle);
        textureGPUHandle.Offset(tableEntry.m_count, m_cbvSrvDescriptorElementSize);
      } else if (renderPassDescriptorCount.m_numSamplerSlots > 0 && tableEntry.m_type == DescriptorType::Sampler) {
        LOG_DEBUG(log_D3D12RenderSystem, LOG_LEVEL, "Setting Sampler Descriptor Table at %d", tableIdx);
        secondaryCommandList->SetComputeRootDescriptorTable(tableIdx, samplerGPUHandle);
        samplerGPUHandle.Offset(tableEntry.m_count, m_samplerDescriptorElementSize);
      } else if (tableEntry.m_type == DescriptorType::UniformBuffer) {
        LOG_DEBUG(log_D3D12RenderSystem, LOG_LEVEL, "Setting Uniform Descriptor Table at %d", tableIdx);
        secondaryCommandList->SetComputeRootDescriptorTable(tableIdx, uboGPUHandle);
        uboGPUHandle.Offset(tableEntry.m_count, m_cbvSrvDescriptorElementSize);
      }

      ++tableIdx;
    }

    if (renderPassInputTargets.GetSize() > 0) {
      secondaryCommandList->SetComputeRootDescriptorTable(renderPass.GetInputTargetRootDescriptorTableId(), inputTargetsGPUHandle);
      LOG_DEBUG(log_D3D12RenderSystem, LOG_LEVEL, "Setting Input Attachment Descriptor Table at %d",
        renderPass.GetInputTargetRootDescriptorTableId());
    }

    if (renderPassInputBuffers.GetSize() > 0) {
      secondaryCommandList->SetComputeRootDescriptorTable(renderPass.GetInputBufferRootDescriptorTableId(), inputBuffersGPUHandle);
      LOG_DEBUG(log_D3D12RenderSystem, LOG_LEVEL, "Setting Input Buffer Descriptor Table at %d",
        renderPass.GetInputBufferRootDescriptorTableId());
    }

    if (renderPassOutputs.GetSize() > 0) {
      secondaryCommandList->SetComputeRootDescriptorTable(renderPass.GetOutputRootDescriptorTableId(), outputsGPUHandle);
      LOG_DEBUG(log_D3D12RenderSystem, LOG_LEVEL, "Setting Input Attachment Descriptor Table at %d",
        renderPass.GetOutputRootDescriptorTableId());
    }

    inputTargetsRecorded += renderPassInputTargets.GetSize();
    inputBuffersRecorded += renderPassInputBuffers.GetSize();
    outputsRecorded += renderPassOutputs.GetSize();
    samplersRecorded += renderPassDescriptorCount.m_numSamplerSlots;
    cbRecorded += renderPassDescriptorCount.m_numUniformSlots;
    sampledImageRecorded += renderPassDescriptorCount.m_numSampledImageSlots;

    LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Drawable Pool: Recording Commands For Dispatch");

    secondaryCommandList->Dispatch(UINT(m_launchDims.m_x), UINT(m_launchDims.m_y), UINT(m_launchDims.m_z));

    ++idx;
  }
}

void D3D12ComputePool::BeginUpdates() {
  m_updateBuffer.Reset();
  m_bufferUpdates.Reset();
}

void D3D12ComputePool::UpdateUniformData(SlotID slot, const U8* buffer, U32 size) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL,
    "D3D12 Drawable Pool: Update Uniform Requested for Slot: %d of Size: %d bytes", slot,
    size);

  size = (size + D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);

  const U32 offset = m_updateBuffer.AppendData(buffer, size, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT, log_D3D12RenderSystem);

  const auto& descriptorSlot = m_globalDescriptorSlots[slot];
  const U32 bufferId = GetSingleUniformBufferInfo(descriptorSlot);

  BufferUpdate info = {};
  info.m_type = DescriptorType::UniformBuffer;
  info.m_idx = bufferId;
  info.m_updateOffset = offset;
  info.m_updateByteSize = size;
  info.m_gpuOffset = m_uniformBufferInfos[bufferId].m_offset;
  info.m_gpuByteSize = m_uniformBufferInfos[bufferId].m_byteSize;

  m_bufferUpdates.PushBack(info);
}

void D3D12ComputePool::UpdateTextureData(SlotID slot, const U8* buffer) {
  const auto& descriptorSlot = m_globalDescriptorSlots[slot];
  const U32 bufferId = GetSingleTextureBufferInfo(descriptorSlot);

  const TextureDesc& desc = m_textureBufferInfos[bufferId].m_desc;

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL,
    "D3D12 Drawable Pool: Updating Texture Requested for Slot: %d of Size: %d bytes", slot, desc.m_size);

  const U32 textureWidthBytes = desc.m_bounds.m_width * GetFormatSize(desc.m_format);

  // TODO(vasumahesh1):[INPUT]: Could be an issue with sizeof(float)
  const U32 offset = m_updateBuffer.AppendTextureData(buffer, desc.m_size, 512, textureWidthBytes, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT, log_D3D12RenderSystem);

  BufferUpdate info = {};
  info.m_type = DescriptorType::SampledImage;
  info.m_idx = bufferId;
  info.m_updateOffset = offset;
  info.m_updateByteSize = desc.m_size;
  info.m_gpuOffset = m_textureBufferInfos[bufferId].m_offset;
  info.m_gpuByteSize = m_textureBufferInfos[bufferId].m_byteSize;

  m_bufferUpdates.PushBack(info);

}

void D3D12ComputePool::SubmitUpdates() {
  auto oneTimeSubmitBuffer = D3D12ScopedCommandBuffer(m_device, D3D12_COMMAND_LIST_TYPE_DIRECT, log_D3D12RenderSystem);
  oneTimeSubmitBuffer.CreateGraphicsCommandList(m_device, nullptr, log_D3D12RenderSystem);

  auto oneTimeCommandList = oneTimeSubmitBuffer.GetGraphicsCommandList();

  m_mainBuffer.Transition(oneTimeCommandList,D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST);

  // Copy Custom Regions
  for(const auto& updateRegion : m_bufferUpdates)
  {
    if (updateRegion.m_type == DescriptorType::UniformBuffer) {
      oneTimeCommandList->CopyBufferRegion(m_mainBuffer.Real(), updateRegion.m_gpuOffset, m_updateBuffer.Real(), updateRegion.m_updateOffset, updateRegion.m_updateByteSize);
    }
    else if (updateRegion.m_type == DescriptorType::SampledImage)
    {
      const auto& targetImage = m_images[updateRegion.m_idx];
      targetImage.Transition(oneTimeCommandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST, log_D3D12RenderSystem);
      targetImage.CopyFromBuffer(m_device, oneTimeCommandList, m_updateBuffer, updateRegion.m_updateOffset);
      targetImage.Transition(oneTimeCommandList, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, log_D3D12RenderSystem);
    }
  }

  m_mainBuffer.Transition(oneTimeCommandList, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

  oneTimeCommandList->Close();

  oneTimeSubmitBuffer.Execute(m_device, m_graphicsCommandQueue.Get(), log_D3D12RenderSystem);
  oneTimeSubmitBuffer.WaitForComplete(m_graphicsCommandQueue.Get(), log_D3D12RenderSystem);
}

const Vector<ID3D12DescriptorHeap*>& D3D12ComputePool::GetAllDescriptorHeaps() const {
  return m_allHeaps;
}

ID3D12PipelineState* D3D12ComputePool::GetPipelineState(U32 renderPassId) const {
  return m_pipelines[renderPassId].GetState();
}

void D3D12ComputePool::CreateRenderPassReferences(const ComputePoolCreateInfo& createInfo,
                                                   const Containers::Vector<D3D12ScopedComputePass>& renderPasses) {

  U32 idx = 0;
  for (const auto& renderPass : renderPasses) {
    auto it = std::find_if(createInfo.m_computePasses.Begin(), createInfo.m_computePasses.End(), [&](U32 passId)
    {
      return renderPass.GetId() == passId;
    });

    if (it != createInfo.m_computePasses.End()) {
      m_computePasses.PushBack(std::reference_wrapper<D3D12ScopedComputePass>(renderPasses[idx]));
    }

    ++idx;
  }
}

void D3D12ComputePool::CreateDescriptorHeap() {
  m_allHeaps.Reserve(2);

  m_offsetToComputePassInputTargets = 0;
  m_offsetToComputePassInputBuffers = 0;
  m_offsetToComputePassOutputs = 0;
  m_offsetToConstantBuffers = 0;

  U32 totalDescriptors = 0;

  U32 numSamplers = 0;

  for (const auto& renderPass : m_computePasses) {
    const auto& count = renderPass.get().GetDescriptorCount();

    const U32 numOutputs = renderPass.get().GetOutputImages().GetSize();
    const U32 numInputTargets = renderPass.get().GetInputImages().GetSize();
    const U32 numInputBuffers = renderPass.get().GetInputBuffers().GetSize();
    const U32 numSRVs = count.m_numSampledImageSlots;
    const U32 numUBO = count.m_numUniformSlots;

    m_offsetToConstantBuffers += numSRVs;
    m_offsetToComputePassInputTargets += numSRVs + numUBO;
    m_offsetToComputePassInputBuffers += numSRVs + numUBO + numInputTargets;
    m_offsetToComputePassOutputs += numSRVs + numUBO + numInputTargets + numInputBuffers;

    totalDescriptors += numOutputs + numInputTargets + numInputBuffers + numSRVs + numUBO;

    numSamplers += count.m_numSamplerSlots;
  }

  D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
  heapDesc.NumDescriptors             = totalDescriptors;
  heapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
  heapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
  VERIFY_D3D_OP(log_D3D12RenderSystem, m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_descriptorComputeHeap)
  ), "Failed to create Drawable Descriptor Heap");
  m_allHeaps.PushBack(m_descriptorComputeHeap.Get());

  if (numSamplers > 0) {
    D3D12_DESCRIPTOR_HEAP_DESC samplerDesc = {};
    samplerDesc.NumDescriptors             = numSamplers;
    samplerDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    samplerDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    VERIFY_D3D_OP(log_D3D12RenderSystem, m_device->CreateDescriptorHeap(&samplerDesc, IID_PPV_ARGS(&
      m_descriptorSamplerHeap)), "Failed to create Sampler Descriptor Heap");

    m_allHeaps.PushBack(m_descriptorSamplerHeap.Get());
  }
}
} // namespace D3D12
} // namespace Azura
