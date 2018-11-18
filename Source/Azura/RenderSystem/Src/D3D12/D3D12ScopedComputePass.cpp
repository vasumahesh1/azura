#include "D3D12/D3D12ScopedComputePass.h"
#include "D3D12/D3D12Macros.h"
#include "Memory/MonotonicAllocator.h"
#include "Memory/MemoryFactory.h"
#include "D3D12/D3D12TypeMapping.h"

using namespace Microsoft::WRL;    // NOLINT
using namespace Azura::Containers; // NOLINT

namespace Azura {
namespace D3D12 {

D3D12ScopedComputePass::D3D12ScopedComputePass(U32 idx, U32 internalId,
                                               Memory::Allocator& mainAllocator,
                                               Log logger)
  : log_D3D12RenderSystem(std::move(logger)),
    m_id(idx),
    m_internalId(internalId),
    m_rootSignatureTable(mainAllocator),
    m_passShaders(mainAllocator),
    m_computeOutputTargets(mainAllocator),
    m_computeOutputBuffers(mainAllocator),
    m_computeInputBuffers(mainAllocator),
    m_computeInputTargets(mainAllocator),
    m_computeDepthInputTargets(mainAllocator),
    m_allComputeInputTargets(mainAllocator) {
}

void D3D12ScopedComputePass::Create(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
                                    const PipelinePassCreateInfo& createInfo,
                                    const Containers::Vector<RenderTargetCreateInfo>& targetCreateInfos,
                                    const Containers::Vector<D3D12ScopedImage>& pipelineImages,
                                    const Containers::Vector<D3D12ScopedBuffer>& pipelineStructuredBuffers,
                                    const Containers::Vector<DescriptorSlot>& descriptorSlots,
                                    const Containers::Vector<DescriptorTableEntry>& descriptorSetTable,
                                    const Containers::Vector<D3D12ScopedShader>& allShaders) {

  CreateBase(device, createInfo, descriptorSlots, descriptorSetTable, targetCreateInfos, pipelineImages, pipelineStructuredBuffers, allShaders);

  D3D12ScopedCommandBuffer primaryCmdBuffer = D3D12ScopedCommandBuffer(device, D3D12_COMMAND_LIST_TYPE_COMPUTE,
                                                                       log_D3D12RenderSystem);
  primaryCmdBuffer.CreateGraphicsCommandList(device, nullptr, log_D3D12RenderSystem);
  m_commandBuffers.push_back(primaryCmdBuffer);
}

U32 D3D12ScopedComputePass::GetId() const {
  return m_id;
}

U32 D3D12ScopedComputePass::GetInternalId() const {
  return m_internalId;
}

ID3D12GraphicsCommandList* D3D12ScopedComputePass::GetPrimaryComputeCommandList(U32 idx) const {
  return m_commandBuffers[idx].GetGraphicsCommandList();
}

const DescriptorCount& D3D12ScopedComputePass::GetDescriptorCount() const {
  return m_descriptorCount;
}

ID3D12RootSignature* D3D12ScopedComputePass::GetRootSignature() const {
  return m_rootSignature.Get();
}

const Vector<std::reference_wrapper<const D3D12ScopedShader>>& D3D12ScopedComputePass::GetShaders() const {
  return m_passShaders;
}

const Vector<std::reference_wrapper<D3D12ScopedImage>>& D3D12ScopedComputePass::GetInputImages() const {
  return m_allComputeInputTargets;
}

const Containers::Vector<std::reference_wrapper<D3D12ScopedBuffer>>& D3D12ScopedComputePass::GetInputBuffers() const {
  return m_computeInputBuffers;
}

const Containers::Vector<std::reference_wrapper<D3D12ScopedBuffer>>& D3D12ScopedComputePass::GetOutputBuffers() const {
  return m_computeOutputBuffers;
}

const Vector<std::reference_wrapper<D3D12ScopedImage>>& D3D12ScopedComputePass::GetOutputImages() const {
  return m_computeOutputTargets;
}

const Vector<DescriptorTableEntry>& D3D12ScopedComputePass::GetRootSignatureTable() const {
  return m_rootSignatureTable;
}

void D3D12ScopedComputePass::RecordResourceBarriersForOutputsStart(ID3D12GraphicsCommandList* commandList) const {
  for (auto& rtv : m_computeOutputTargets) {
    rtv.get().Transition(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, log_D3D12RenderSystem);
  }
  
  for (auto& buffer : m_computeOutputBuffers) {
    buffer.get().Transition(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, log_D3D12RenderSystem);
  }
}
  
void D3D12ScopedComputePass::RecordResourceBarriersForOutputsEnd(ID3D12GraphicsCommandList* commandList) const {
  for (auto& rtv : m_computeOutputTargets) {
    rtv.get().Transition(commandList, D3D12_RESOURCE_STATE_COMMON, log_D3D12RenderSystem);
  }

  for (auto& buffer : m_computeOutputBuffers) {
    buffer.get().Transition(commandList, D3D12_RESOURCE_STATE_COMMON, log_D3D12RenderSystem);
  }
}

void D3D12ScopedComputePass::RecordResourceBarriersForInputsStart(ID3D12GraphicsCommandList* commandList) const {
  for (auto& rtv : m_computeInputTargets) {
    rtv.get().Transition(commandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, log_D3D12RenderSystem);
  }

  for (auto& dsv : m_computeDepthInputTargets) {
    dsv.get().Transition(commandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, log_D3D12RenderSystem);
  }
}
  
void D3D12ScopedComputePass::RecordResourceBarriersForInputsEnd(ID3D12GraphicsCommandList* commandList) const {
  for (auto& rtv : m_computeInputTargets) {
    rtv.get().Transition(commandList, D3D12_RESOURCE_STATE_COMMON, log_D3D12RenderSystem);
  }

  for (auto& dsv : m_computeDepthInputTargets) {
    dsv.get().Transition(commandList, D3D12_RESOURCE_STATE_COMMON, log_D3D12RenderSystem);
  }
}

void D3D12ScopedComputePass::WaitForGPU(ID3D12CommandQueue* commandQueue) {
  const U32 fence = m_fenceValue;
  VERIFY_D3D_OP(log_D3D12RenderSystem, commandQueue->Signal(m_signalFence.Get(), m_fenceValue), "Fence wait failed");
  m_fenceValue++;

  // Wait until the previous frame is finished.
  if (m_signalFence->GetCompletedValue() < fence) {
    VERIFY_D3D_OP(log_D3D12RenderSystem, m_signalFence->SetEventOnCompletion(fence, m_fenceEvent),
      "Failed to set event completion on Fence");
    WaitForSingleObject(m_fenceEvent, INFINITE);
  }

}

U32 D3D12ScopedComputePass::GetCommandBufferCount() const {
  return U32(m_commandBuffers.size());
}

U32 D3D12ScopedComputePass::GetInputTargetRootDescriptorTableId() const {
  return m_computeInputTargetTableIdx;
}

U32 D3D12ScopedComputePass::GetInputBufferRootDescriptorTableId() const {
  return m_computeInputBufferTableIdx;
}

U32 D3D12ScopedComputePass::GetOutputTargetsRootDescriptorTableId() const {
  return m_computeOutputTargetTableIdx;
}

U32 D3D12ScopedComputePass::GetOutputBuffersRootDescriptorTableId() const {
  return m_computeOutputBufferTableIdx;
}

void D3D12ScopedComputePass::CreateBase(
  const Microsoft::WRL::ComPtr<ID3D12Device>& device,
  const PipelinePassCreateInfo& createInfo,
  const Containers::Vector<DescriptorSlot>& descriptorSlots,
  const Containers::Vector<DescriptorTableEntry>& descriptorSetTable,
  const Containers::Vector<RenderTargetCreateInfo>& targetCreateInfos,
  const Containers::Vector<D3D12ScopedImage>& pipelineImages,
  const Containers::Vector<D3D12ScopedBuffer>& pipelineStructuredBuffers,
  const Containers::Vector<D3D12ScopedShader>& allShaders) {

  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 2048);

  VERIFY_D3D_OP(log_D3D12RenderSystem, device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_signalFence)),
    "Failed to create fence");

  // Create an event handle to use for frame synchronization.
  m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
  if (m_fenceEvent == nullptr) {
    VERIFY_D3D_OP(log_D3D12RenderSystem, HRESULT_FROM_WIN32(GetLastError()), "Fence Event Null");
  }

  m_passShaders.Reserve(U32(createInfo.m_shaders.size()));
  m_computeInputBuffers.Reserve(U32(createInfo.m_inputBuffers.size()));
  m_computeInputTargets.Reserve(U32(createInfo.m_inputTargets.size()));
  m_computeDepthInputTargets.Reserve(U32(createInfo.m_inputTargets.size()));
  m_allComputeInputTargets.Reserve(U32(createInfo.m_inputTargets.size()));
  m_computeOutputTargets.Reserve(U32(createInfo.m_outputTargets.size()));
  m_computeOutputBuffers.Reserve(U32(createInfo.m_outputBuffers.size()));

  for (const auto& outputId : createInfo.m_outputTargets) {
    m_computeOutputTargets.PushBack(pipelineImages[outputId]);
  }
  
  for (const auto& outputId : createInfo.m_outputBuffers) {
    m_computeOutputBuffers.PushBack(pipelineStructuredBuffers[outputId]);
  }

  for (const auto& shaderId : createInfo.m_shaders) {
    const auto& shaderRef = allShaders[shaderId];
    m_passShaders.PushBack(shaderRef);
  }

  for (const auto& inputId : createInfo.m_inputTargets) {
    // TODO(vasumahesh1):[?]: Respect shader stages here for correct binding
    const auto& targetBufferRef = targetCreateInfos[inputId.m_id];

    if (HasDepthOrStencilComponent(targetBufferRef.m_format)) {
      m_computeDepthInputTargets.PushBack(pipelineImages[inputId.m_id]);
    } else {
      m_computeInputTargets.PushBack(pipelineImages[inputId.m_id]);
    }

    m_allComputeInputTargets.PushBack(pipelineImages[inputId.m_id]);
  }

  for (const auto& inputBuffer : createInfo.m_inputBuffers) {
    m_computeInputBuffers.PushBack(pipelineStructuredBuffers[inputBuffer.m_id]);
  }

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "======== D3D12 Render Pass: Root Signature ========");

  Vector<CD3DX12_ROOT_PARAMETER> descriptorTables(U32(createInfo.m_descriptorSets.size() + 2), allocatorTemporary);

  m_rootSignatureTable.Reserve(U32(createInfo.m_descriptorSets.size()));

  U32 cbvOffset     = 0;
  U32 srvOffset     = 0;
  U32 uavOffset     = 0;
  U32 samplerOffset = 0;

  for (const auto& setId : createInfo.m_descriptorSets) {
    const auto& tableEntry = descriptorSetTable[setId];

    LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Render Pass: Generating for Set Position: %d", m_rootSignatureTable
      .GetSize());
    LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Render Pass: Original Set Position: %d", setId);

    m_rootSignatureTable.PushBack(tableEntry);

    Vector<CD3DX12_DESCRIPTOR_RANGE> currentRanges(tableEntry.m_count, allocatorTemporary);

    for (const auto& slot : descriptorSlots) {
      if (slot.m_setIdx != setId) {
        continue;
      }

      CD3DX12_DESCRIPTOR_RANGE rangeData;

      switch (slot.m_type) {
        case DescriptorType::UniformBuffer:
          LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Render Pass: [%d] Applying Uniform Buffer at Register b(%d)",
            setId, cbvOffset);

          rangeData.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, cbvOffset);
          currentRanges.PushBack(rangeData);
          ++cbvOffset;

          ++m_descriptorCount.m_numUniformSlots;
          break;

        case DescriptorType::Sampler:
          LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Render Pass: [%d] Applying Sampler Buffer at Register s(%d)",
            setId, samplerOffset);

          rangeData.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, samplerOffset);
          currentRanges.PushBack(rangeData);
          ++samplerOffset;

          ++m_descriptorCount.m_numSamplerSlots;
          break;

        case DescriptorType::SampledImage:
          LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Render Pass: [%d] Applying Texture Image at Register t(%d)",
            setId, srvOffset);

          rangeData.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, srvOffset);
          currentRanges.PushBack(rangeData);
          ++srvOffset;

          ++m_descriptorCount.m_numSampledImageSlots;
          break;

        case DescriptorType::UnorderedView:
          LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Render Pass: [%d] Applying UAV at Register t(%d)",
            setId, uavOffset);

          rangeData.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, uavOffset);
          currentRanges.PushBack(rangeData);
          ++uavOffset;

          ++m_descriptorCount.m_numUnorderedViewSlots;
          break;

        case DescriptorType::PushConstant:
        case DescriptorType::CombinedImageSampler:
        default:
          LOG_ERR(log_D3D12RenderSystem, LOG_LEVEL, "Unsupported Descriptor Type for D3D12");
          break;
      }
    }

    CD3DX12_ROOT_PARAMETER rootParameter;
    rootParameter.InitAsDescriptorTable(currentRanges.GetSize(), currentRanges.Data(), D3D12_SHADER_VISIBILITY_ALL);
    descriptorTables.PushBack(rootParameter);
  }

  CD3DX12_ROOT_PARAMETER inputsRootParameter;
  CD3DX12_ROOT_PARAMETER ouputsRootParameter;
  CD3DX12_DESCRIPTOR_RANGE inputTargetRangeData;
  CD3DX12_DESCRIPTOR_RANGE inputBufferRangeData;
  CD3DX12_DESCRIPTOR_RANGE outputRangeData;

  // Have some inputs targets
  if (!createInfo.m_inputTargets.empty()) {
    LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Render Pass: Generating SRV for Set Position: %d", descriptorTables.
      GetSize());
    LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL,
      "D3D12 Render Pass: [Attachments] Applying %d Image Attachments as register t(%d) to t(%d)", createInfo.m_inputTargets.
      size(), srvOffset, srvOffset + createInfo.m_inputTargets.size() - 1);

    inputTargetRangeData.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, UINT(createInfo.m_inputTargets.size()), srvOffset);
    inputsRootParameter.InitAsDescriptorTable(1, &inputTargetRangeData, D3D12_SHADER_VISIBILITY_ALL);

    m_computeInputTargetTableIdx = descriptorTables.GetSize();

    descriptorTables.PushBack(inputsRootParameter);

    srvOffset += U32(createInfo.m_inputTargets.size());
  }

  // Have some inputs buffers
  if (!createInfo.m_inputBuffers.empty()) {
    LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Render Pass: Generating SRV for Set Position: %d", descriptorTables.
      GetSize());
    LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL,
      "D3D12 Render Pass: [Attachments] Applying %d Buffer Attachments as register t(%d) to t(%d)", createInfo.m_inputBuffers.
      size(), srvOffset, srvOffset + createInfo.m_inputBuffers.size() - 1);

    inputBufferRangeData.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, UINT(createInfo.m_inputBuffers.size()), srvOffset);
    inputsRootParameter.InitAsDescriptorTable(1, &inputBufferRangeData, D3D12_SHADER_VISIBILITY_ALL);

    m_computeInputBufferTableIdx = descriptorTables.GetSize();

    descriptorTables.PushBack(inputsRootParameter);
    srvOffset += U32(createInfo.m_inputBuffers.size()); // NOLINT
  }

  if (!createInfo.m_outputTargets.empty()) {
    LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Render Pass: Generating UAV for Set Position: %d", descriptorTables.
      GetSize());
    LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL,
      "D3D12 Render Pass: [Output Attachments] Applying %d Image Attachments as register t(%d) to t(%d)", createInfo.m_outputTargets.
      size(), uavOffset, uavOffset + createInfo.m_outputTargets.size() - 1);

    outputRangeData.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, UINT(createInfo.m_outputTargets.size()), uavOffset);
    ouputsRootParameter.InitAsDescriptorTable(1, &outputRangeData, D3D12_SHADER_VISIBILITY_ALL);

    m_computeOutputTargetTableIdx = descriptorTables.GetSize();

    descriptorTables.PushBack(ouputsRootParameter);
  }

  if (!createInfo.m_outputBuffers.empty()) {
    LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Render Pass: Generating UAV for Set Position: %d", descriptorTables.
      GetSize());
    LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL,
      "D3D12 Render Pass: [Output Attachments] Applying %d Buffer Attachments as register t(%d) to t(%d)", createInfo.m_outputTargets.
      size(), uavOffset, uavOffset + createInfo.m_outputBuffers.size() - 1);

    outputRangeData.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, UINT(createInfo.m_outputBuffers.size()), uavOffset);
    ouputsRootParameter.InitAsDescriptorTable(1, &outputRangeData, D3D12_SHADER_VISIBILITY_ALL);

    m_computeOutputBufferTableIdx = descriptorTables.GetSize();

    descriptorTables.PushBack(ouputsRootParameter);
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

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Render Pass: Completed Root Signature");

}
} // namespace D3D12
} // namespace Azura
