#include "D3D12/D3D12ScopedRenderPass.h"
#include "D3D12/D3D12Macros.h"
#include "Memory/MonotonicAllocator.h"
#include "Memory/MemoryFactory.h"
#include "D3D12/D3D12TypeMapping.h"

using namespace Microsoft::WRL;    // NOLINT
using namespace Azura::Containers; // NOLINT

namespace Azura {
namespace D3D12 {

D3D12ScopedRenderPass::D3D12ScopedRenderPass(U32 idx, const D3D12ScopedSwapChain& swapChain, Memory::Allocator& mainAllocator, Log logger)
  : log_D3D12RenderSystem(std::move(logger)),
    m_id(idx),
    m_swapChainRef(swapChain),
    m_rootSignatureTable(mainAllocator),
    m_renderOutputInfo(mainAllocator),
    m_passShaders(mainAllocator),
    m_passInputs(mainAllocator),
    m_allRenderInputs(mainAllocator) {
}

void D3D12ScopedRenderPass::Create(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
                                   const PipelinePassCreateInfo& createInfo,
                                   const Containers::Vector<RenderTargetCreateInfo>& pipelineBuffers,
                                   const Containers::Vector<D3D12ScopedImage>& pipelineBufferImages,
                                   const Containers::Vector<DescriptorSlot>& descriptorSlots,
                                   const Containers::Vector<DescriptorTableEntry>& descriptorSetTable,
                                   const Containers::Vector<D3D12ScopedShader>& allShaders,
                                   UINT rtvDescriptorSize,
                                   UINT dsvDescriptorSize) {

  CreateBase(device, createInfo, descriptorSlots, descriptorSetTable, pipelineBuffers, pipelineBufferImages,
             allShaders);

  U32 numDSV = 0;
  U32 numRTV = 0;
  for (const auto& output : createInfo.m_outputs) {
    const auto& targetBuffer = pipelineBuffers[output];

    if (HasDepthOrStencilComponent(targetBuffer.m_format)) {
      ++numDSV;
      continue;
    }

    ++numRTV;
  }

  D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
  rtvHeapDesc.NumDescriptors             = numRTV;
  rtvHeapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
  rtvHeapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  VERIFY_D3D_OP(log_D3D12RenderSystem, device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)),
    "Failed to create RTV Heap for Render Pass");

  D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
  dsvHeapDesc.NumDescriptors             = numDSV;
  dsvHeapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
  dsvHeapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  VERIFY_D3D_OP(log_D3D12RenderSystem, device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)),
    "Failed to create DSV Heap for Render Pass");

  CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
  CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

  for (const auto& output : createInfo.m_outputs) {
    const auto& targetBuffer = pipelineBuffers[output];
    auto& gpuTexture   = pipelineBufferImages[output];

    if (HasDepthOrStencilComponent(targetBuffer.m_format)) {
      const auto dsvDesc = D3D12ScopedImage::GetDSV(targetBuffer.m_format, ImageViewType::ImageView2D,
                                                    log_D3D12RenderSystem);
      device->CreateDepthStencilView(gpuTexture.Real(), &dsvDesc, dsvHandle);
      dsvHandle.Offset(1, dsvDescriptorSize);

      hasDepth = true;

      m_depthOutputs.push_back(gpuTexture);
      continue;
    }

    m_renderOutputs.push_back(gpuTexture);

    const auto rtvDesc = D3D12ScopedImage::GetRTV(targetBuffer.m_format, ImageViewType::ImageView2D,
                                                  log_D3D12RenderSystem);

    device->CreateRenderTargetView(gpuTexture.Real(), &rtvDesc, rtvHandle);
    rtvHandle.Offset(1, rtvDescriptorSize);
  }

  D3D12ScopedCommandBuffer primaryCmdBuffer = D3D12ScopedCommandBuffer(device, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                                       log_D3D12RenderSystem);
  primaryCmdBuffer.CreateGraphicsCommandList(device, nullptr, log_D3D12RenderSystem);
  m_commandBuffers.push_back(primaryCmdBuffer);
}

void D3D12ScopedRenderPass::CreateForSwapChain(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
                                               const PipelinePassCreateInfo& createInfo,
                                               const Containers::Vector<RenderTargetCreateInfo>& pipelineBuffers,
                                               const Containers::Vector<D3D12ScopedImage>& pipelineBufferImages,
                                               const Containers::Vector<DescriptorSlot>& descriptorSlots,
                                               const Containers::Vector<DescriptorTableEntry>& descriptorSetTable,
                                               const Containers::Vector<D3D12ScopedShader>& allShaders,
                                               UINT rtvDescriptorSize,
                                               UINT dsvDescriptorSize) {
  UNUSED(dsvDescriptorSize);

  CreateBase(device, createInfo, descriptorSlots, descriptorSetTable, pipelineBuffers, pipelineBufferImages,
             allShaders);

  hasDepth          = false;
  m_isTargetSwapChain = true;

  D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
  rtvHeapDesc.NumDescriptors             = GLOBAL_INFLIGHT_FRAMES;
  rtvHeapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
  rtvHeapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  VERIFY_D3D_OP(log_D3D12RenderSystem, device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)),
    "Failed to create RTV Heap for Swap Chain");

  D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
  rtvHeapDesc.NumDescriptors             = GLOBAL_INFLIGHT_FRAMES;
  rtvHeapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
  rtvHeapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  VERIFY_D3D_OP(log_D3D12RenderSystem, device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)),
    "Failed to create DSV Heap for Swap Chain");

  // Create Frame buffers
  CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

  // Create a RTV for each frame.
  for (U32 idx                                          = 0; idx < GLOBAL_INFLIGHT_FRAMES; idx++) {
    Microsoft::WRL::ComPtr<ID3D12Resource> renderTarget = {};
    VERIFY_D3D_OP(log_D3D12RenderSystem, m_swapChainRef.get().RealComPtr()->GetBuffer(idx, IID_PPV_ARGS(&renderTarget)),
      "Failed to get swapchain buffer");

    m_swapChainImageResources.push_back(renderTarget.Get());

    device->CreateRenderTargetView(renderTarget.Get(), nullptr, rtvHandle);
    rtvHandle.Offset(1, rtvDescriptorSize);
  }

  for (U32 idx                                = 0; idx < GLOBAL_INFLIGHT_FRAMES; ++idx) {
    D3D12ScopedCommandBuffer primaryCmdBuffer = D3D12ScopedCommandBuffer(device, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                                         log_D3D12RenderSystem);
    primaryCmdBuffer.CreateGraphicsCommandList(device, nullptr, log_D3D12RenderSystem);
    m_commandBuffers.push_back(primaryCmdBuffer);
  }
}

U32 D3D12ScopedRenderPass::GetId() const {
  return m_id;
}

ID3D12GraphicsCommandList* D3D12ScopedRenderPass::GetPrimaryGraphicsCommandList(U32 idx) const {
  return m_commandBuffers[idx].GetGraphicsCommandList();
}

const DescriptorCount& D3D12ScopedRenderPass::GetDescriptorCount() const {
  return m_descriptorCount;
}

ID3D12RootSignature* D3D12ScopedRenderPass::GetRootSignature() const {
  return m_rootSignature.Get();
}

void D3D12ScopedRenderPass::RecordImageAcquireBarrier(ID3D12GraphicsCommandList* commandList, U32 idx) const {

  CD3DX12_RESOURCE_BARRIER startBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
                                                                               m_swapChainImageResources[idx],
                                                                               D3D12_RESOURCE_STATE_PRESENT,
                                                                               D3D12_RESOURCE_STATE_RENDER_TARGET);

  commandList->ResourceBarrier(1, &startBarrier);
}

void D3D12ScopedRenderPass::RecordPresentBarrier(ID3D12GraphicsCommandList* commandList, U32 idx) const {
  CD3DX12_RESOURCE_BARRIER endBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
    m_swapChainImageResources[idx],
                                                                             D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                                             D3D12_RESOURCE_STATE_PRESENT);

  commandList->ResourceBarrier(1, &endBarrier);
}

void D3D12ScopedRenderPass::SetRenderTargets(ID3D12GraphicsCommandList* commandList, U32 cBufferIdx, UINT rtvDescriptorSize) const {

  CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), cBufferIdx, rtvDescriptorSize);

  const U32 numRTV = m_isTargetSwapChain ? 1u : U32(m_renderOutputs.size());

  // Set Both Depth & RTV
  if (hasDepth) {
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
    commandList->OMSetRenderTargets(numRTV, &rtvHandle, 1, &dsvHandle);
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
                                       m_clearData.m_depth, m_clearData.m_stencil, 0, nullptr);
  }
    // Set Only RTV
  else {
    commandList->OMSetRenderTargets(numRTV, &rtvHandle, 1, nullptr);
  }

  commandList->ClearRenderTargetView(rtvHandle, m_clearData.m_color, 0, nullptr); // NOLINT
}

const Containers::Vector<std::reference_wrapper<const D3D12ScopedShader>>& D3D12ScopedRenderPass::GetShaders() const {
  return m_passShaders;
}

const Vector<std::reference_wrapper<const RenderTargetCreateInfo>>& D3D12ScopedRenderPass::
GetInputInfo() const {
  return m_passInputs;
}

const Vector<std::reference_wrapper<const D3D12ScopedImage>>& D3D12ScopedRenderPass::GetInputImages() const {
  return m_allRenderInputs;
}


const Containers::Vector<DescriptorTableEntry>& D3D12ScopedRenderPass::GetRootSignatureTable() const {
  return m_rootSignatureTable;
}

void D3D12ScopedRenderPass::RecordResourceBarriersForOutputs(ID3D12GraphicsCommandList* commandList) const {
  for (const auto& rtv : m_renderOutputs) {
    rtv.get().Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
  }

  for (const auto& dsv : m_depthOutputs) {
    dsv.get().Transition(commandList, D3D12_RESOURCE_STATE_DEPTH_WRITE);
  }
}

void D3D12ScopedRenderPass::RecordResourceBarriersForReadingInputs(ID3D12GraphicsCommandList* commandList) const {
  for (const auto& rtv : m_renderInputs) {
    rtv.get().Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);
  }

  for (const auto& dsv : m_depthInputs) {
    dsv.get().Transition(commandList, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
  }
}

void D3D12ScopedRenderPass::RecordResourceBarriersForWritingInputs(ID3D12GraphicsCommandList* commandList) const {
  for (const auto& rtv : m_renderInputs) {
    rtv.get().Transition(commandList, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);
  }

  for (const auto& dsv : m_depthInputs) {
    dsv.get().Transition(commandList, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);
  }
}

void D3D12ScopedRenderPass::WaitForGPU(ID3D12CommandQueue* commandQueue) {
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
U32 D3D12ScopedRenderPass::GetCommandBufferCount() const {
  return U32(m_commandBuffers.size());
}

void D3D12ScopedRenderPass::UpdatePipelineInfo(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc) {
  if (m_isTargetSwapChain)
  {
    const auto format = ToDXGI_FORMAT(m_swapChainRef.get().GetFormat());
    VERIFY_OPT(log_D3D12RenderSystem, format, "Unknown Format - Swap Chain - For Pipeline Render Target Format");


    psoDesc.RTVFormats[0] = format.value();
    psoDesc.NumRenderTargets = 1;
    return;
  }
  
  U32 rtvIdx = 0;
  for(const auto& outputInfo : m_renderOutputInfo)
  {
    const auto format = ToDXGI_FORMAT(outputInfo.m_format);
    VERIFY_OPT(log_D3D12RenderSystem, format, "Unknown Format For Pipeline Render Target Format");

    if (HasDepthOrStencilComponent(outputInfo.m_format))
    {
      psoDesc.DSVFormat = format.value();
      continue;
    }

    psoDesc.RTVFormats[rtvIdx] = format.value(); // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
    ++rtvIdx;
  }

  psoDesc.NumRenderTargets = m_renderOutputInfo.GetSize();
}

void D3D12ScopedRenderPass::CreateBase(
  const Microsoft::WRL::ComPtr<ID3D12Device>& device,
  const PipelinePassCreateInfo& createInfo,
  const Containers::Vector<DescriptorSlot>& descriptorSlots,
  const Containers::Vector<DescriptorTableEntry>& descriptorSetTable,
  const Containers::Vector<RenderTargetCreateInfo>& pipelineBuffers,
  const Containers::Vector<D3D12ScopedImage>& pipelineBufferImages,
  const Containers::Vector<D3D12ScopedShader>& allShaders) {

  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 2048);

  VERIFY_D3D_OP(log_D3D12RenderSystem, device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_signalFence)),
    "Failed to create fence");

  m_blendState = createInfo.m_blendState;

  m_clearData = createInfo.m_clearData;

  // Create an event handle to use for frame synchronization.
  m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
  if (m_fenceEvent == nullptr) {
    VERIFY_D3D_OP(log_D3D12RenderSystem, HRESULT_FROM_WIN32(GetLastError()), "Fence Event Null");
  }

  m_passShaders.Reserve(U32(createInfo.m_shaders.size()));
  m_passInputs.Reserve(U32(createInfo.m_inputs.size()));
  m_renderOutputInfo.Reserve(U32(createInfo.m_outputs.size()));
  m_allRenderInputs.Reserve(U32(createInfo.m_inputs.size()));

  for(const auto& outputId : createInfo.m_outputs)
  {
    const auto& targetBufferRef = pipelineBuffers[outputId];
    m_renderOutputInfo.PushBack({ targetBufferRef.m_format });
  }

  for (const auto& shaderId : createInfo.m_shaders) {
    const auto& shaderRef = allShaders[shaderId];
    m_passShaders.PushBack(shaderRef);
  }

  for (const auto& inputId : createInfo.m_inputs) {
    // TODO(vasumahesh1):[?]: Respect shader stages here for correct binding
    const auto& targetBufferRef = pipelineBuffers[inputId.m_id];
    m_passInputs.PushBack(targetBufferRef);

    if (HasDepthOrStencilComponent(targetBufferRef.m_format)) {
      m_depthInputs.push_back(pipelineBufferImages[inputId.m_id]);
    } else {
      m_renderInputs.push_back(pipelineBufferImages[inputId.m_id]);
    }

    m_allRenderInputs.PushBack(pipelineBufferImages[inputId.m_id]);
  }

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "======== D3D12 Render Pass: Root Signature ========");

  Vector<CD3DX12_ROOT_PARAMETER> descriptorTables(U32(createInfo.m_descriptorSets.size() + 1), allocatorTemporary);

  m_rootSignatureTable.Reserve(U32(createInfo.m_descriptorSets.size()));

  U32 cbvOffset     = 0;
  U32 srvOffset     = 0;
  U32 samplerOffset = 0;

  for (const auto& setId : createInfo.m_descriptorSets) {
    const auto& tableEntry = descriptorSetTable[setId];

    LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Render Pass: Generating for Set Position: %d", m_rootSignatureTable.GetSize());
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
          LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Render Pass: [%d] Applying Uniform Buffer at Register b(%d)", setId, cbvOffset);

          rangeData.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, cbvOffset);
          currentRanges.PushBack(rangeData);
          ++cbvOffset;

          ++m_descriptorCount.m_numUniformSlots;
          break;

        case DescriptorType::Sampler:
          LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Render Pass: [%d] Applying Sampler Buffer at Register s(%d)", setId, samplerOffset);

          rangeData.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, samplerOffset);
          currentRanges.PushBack(rangeData);
          ++samplerOffset;

          ++m_descriptorCount.m_numSamplerSlots;
          break;

        case DescriptorType::SampledImage:
          LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Render Pass: [%d] Applying Texture Image at Register t(%d)", setId, srvOffset);

          rangeData.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, srvOffset);
          currentRanges.PushBack(rangeData);
          ++srvOffset;

          ++m_descriptorCount.m_numSampledImageSlots;
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
  CD3DX12_DESCRIPTOR_RANGE inputRangeData;

  // Have some inputs
  if (!createInfo.m_inputs.empty())
  {
    LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Render Pass: Generating for Set Position: %d", descriptorTables.GetSize());
    LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12 Render Pass: [Attachments] Applying %d Image Attachments as register t(%d) to t(%d)", createInfo.m_inputs.size(), srvOffset, srvOffset + createInfo.m_inputs.size() - 1);

    inputRangeData.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, UINT(createInfo.m_inputs.size()), srvOffset);
    inputsRootParameter.InitAsDescriptorTable(1, &inputRangeData, D3D12_SHADER_VISIBILITY_ALL);

    descriptorTables.PushBack(inputsRootParameter);
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
