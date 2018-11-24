#include "D3D12/D3D12Renderer.h"
#include "D3D12/D3D12Macros.h"

#include "D3D12/d3dx12.h"
#include "Memory/MemoryFactory.h"
#include <array>
#include "Log/Log.h"

using namespace Microsoft::WRL;    // NOLINT
using namespace Azura::Containers; // NOLINT

namespace Azura {
namespace D3D12 {

D3D12Renderer::D3D12Renderer(const ApplicationInfo& appInfo,
                             const DeviceRequirements& deviceRequirements,
                             const ApplicationRequirements& appRequirements,
                             const SwapChainRequirements& swapChainRequirements,
                             const RenderPassRequirements& renderPassRequirements,
                             const DescriptorRequirements& descriptorRequirements,
                             const ShaderRequirements& shaderRequirements,
                             Memory::Allocator& mainAllocator,
                             Memory::Allocator& drawAllocator,
                             Window& window)
  : Renderer(appInfo, deviceRequirements, appRequirements, swapChainRequirements, descriptorRequirements,
             mainAllocator, drawAllocator, window),
    log_D3D12RenderSystem(Log("D3D12RenderSystem")),
    m_perFrameBuffer(8192),
    m_perFrameAllocator(m_perFrameBuffer, 8192),
    m_initBuffer(0x400000),
    m_initAllocator(m_initBuffer, 0x400000),
    m_renderSequence(renderPassRequirements.m_passSequence.GetSize(), mainAllocator),
    m_renderTargetUpdates(renderPassRequirements.m_targets.GetSize(), mainAllocator),
    m_bufferTargetUpdates(renderPassRequirements.m_buffers.GetSize(), mainAllocator),
    m_targetImages(renderPassRequirements.m_targets.GetSize(), mainAllocator),
    m_targetBuffers(renderPassRequirements.m_buffers.GetSize(), mainAllocator),
    m_renderPasses(renderPassRequirements.m_passSequence.GetSize(), mainAllocator),
    m_computePasses(renderPassRequirements.m_passSequence.GetSize(), mainAllocator),
    m_drawablePools(renderPassRequirements.m_maxPools, drawAllocator),
    m_computePools(renderPassRequirements.m_maxPools, drawAllocator),
    m_shaders(shaderRequirements.m_shaders.GetSize(), mainAllocator) {

  const auto viewportDimensions = window.GetViewport();
  m_viewport                    = CD3DX12_VIEWPORT(0.0f, 0.0f, viewportDimensions.m_width, viewportDimensions.m_height);
  m_scissorRect                 = CD3DX12_RECT(0, 0, static_cast<LONG>(viewportDimensions.m_width),
                                               static_cast<LONG>(viewportDimensions.m_height));

#ifdef BUILD_DEBUG
  UINT dxgiFactoryFlags = 0;
  ComPtr<ID3D12Debug> debugController;
  ComPtr<ID3D12Debug1> debugController1;
  if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
    debugController->EnableDebugLayer();
  
    debugController->QueryInterface(IID_PPV_ARGS(&debugController1));
    debugController1->SetEnableGPUBasedValidation(true); // NOLINT
  
    // Enable additional debug layers.
    dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
  }
#elif defined(BUILD_RELEASE)
  const UINT dxgiFactoryFlags = 0;
#endif

  ComPtr<IDXGIFactory4> factory;
  VERIFY_D3D_OP(log_D3D12RenderSystem, CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)),
    "Failed to create DXGI Factory");

  ComPtr<IDXGIAdapter1> hardwareAdapter;
  D3D12Core::GetHardwareAdapter(factory.Get(), &hardwareAdapter, log_D3D12RenderSystem);

#ifdef BUILD_DEBUG
  ComPtr<IDXGIAdapter> warpAdapter;
  VERIFY_D3D_OP(log_D3D12RenderSystem, factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)), "Failed to create Warp adapter");
#endif

  VERIFY_D3D_OP(log_D3D12RenderSystem, D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&
    m_device)), "Failed to create D3D12 Device");

  // Describe and create the command queue.
  D3D12_COMMAND_QUEUE_DESC queueDesc = {};
  queueDesc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
  queueDesc.Type                     = D3D12_COMMAND_LIST_TYPE_DIRECT;

  VERIFY_D3D_OP(log_D3D12RenderSystem, m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_mainGraphicsCommandQueue)),
    "Failed to create command Queue");

  D3D12_COMMAND_QUEUE_DESC computeQueueDesc = {};
  computeQueueDesc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
  computeQueueDesc.Type                     = D3D12_COMMAND_LIST_TYPE_COMPUTE;

  VERIFY_D3D_OP(log_D3D12RenderSystem, m_device->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&m_mainComputeCommandQueue)),
    "Failed to create command Queue");

  m_swapChain.Create(factory, m_mainGraphicsCommandQueue, m_window.get(), swapChainRequirements, log_D3D12RenderSystem);

  SetCurrentFrame(m_swapChain.RealComPtr()->GetCurrentBackBufferIndex());

  // Create Buffers
  m_stagingBuffer.Create(m_device, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), GetApplicationRequirements().m_renderer.m_stagingBufferSize,
    D3D12_RESOURCE_STATE_GENERIC_READ, log_D3D12RenderSystem);
  m_stagingBuffer.Map();


  for (const auto& shaderCreateInfo : shaderRequirements.m_shaders) {
    D3D12Renderer::AddShader(shaderCreateInfo);
  }

  for (const auto& renderTarget : renderPassRequirements.m_targets) {
    TextureDesc desc = {};
    desc.m_format    = renderTarget.m_format;
    desc.m_bounds    = Bounds3D{swapChainRequirements.m_extent.m_width, swapChainRequirements.m_extent.m_height, 1};

    if (renderTarget.m_width > 0 && renderTarget.m_height > 0 && renderTarget.m_depth > 0)
    {
      desc.m_bounds = Bounds3D{U32(renderTarget.m_width), U32(renderTarget.m_height), U32(renderTarget.m_depth)};
    }

    LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "Creating Attachment Input at: %d for %s", m_targetImages.
      GetSize(), ToString(renderTarget.m_format).c_str());

    const D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON;

    D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    if (HasDepthComponent(desc.m_format) || HasStencilComponent(desc.m_format)) {
      resourceFlags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    }

    m_targetImages.PushBack(D3D12ScopedImage());
    m_targetImages.Last().Create(m_device, resourceState, resourceFlags, desc, log_D3D12RenderSystem);
  }

  for (const auto& bufferTarget : renderPassRequirements.m_buffers) {
    const D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON;
    const D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    m_targetBuffers.PushBack(D3D12ScopedBuffer());
    m_targetBuffers.Last().Create(m_device, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), bufferTarget.m_size, resourceState, resourceFlags, log_D3D12RenderSystem);
    m_targetBuffers.Last().SetStride(bufferTarget.m_stride);
  }

  if (swapChainRequirements.m_depthFormat != RawStorageFormat::UNKNOWN) {
    TextureDesc desc = {};
    desc.m_format = swapChainRequirements.m_depthFormat;
    desc.m_bounds = Bounds3D{ swapChainRequirements.m_extent.m_width, swapChainRequirements.m_extent.m_height, 1 };

    const D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    const D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    m_depthTexture.Create(m_device, resourceState, resourceFlags, desc, log_D3D12RenderSystem);
  }

  m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
  m_dsvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

  U32 passCount = 0;
  for (const auto& passCreateInfo : renderPassRequirements.m_passSequence) {
    if (passCreateInfo.m_type == RenderPassType::Graphics) {

      D3D12ScopedRenderPass renderPass = D3D12ScopedRenderPass(passCount, m_renderPasses.GetSize(), m_swapChain, mainAllocator,
        log_D3D12RenderSystem);

      if (passCount != (renderPassRequirements.m_passSequence.GetSize() - 1)) {
        renderPass.Create(m_device,
          passCreateInfo,
          renderPassRequirements.m_targets,
          m_targetImages,
          m_descriptorSlots,
          m_descriptorSetTable,
          m_shaders,
          m_rtvDescriptorSize, m_dsvDescriptorSize);
      }
      else {
        renderPass.CreateForSwapChain(m_device,
          passCreateInfo,
          renderPassRequirements.m_targets,
          m_targetImages,
          m_depthTexture,
          m_descriptorSlots,
          m_descriptorSetTable,
          m_shaders,
          m_rtvDescriptorSize, m_dsvDescriptorSize);
      }

      m_renderPasses.PushBack(renderPass);
      m_renderSequence.PushBack(std::make_pair(m_renderPasses.GetSize() - 1, passCreateInfo.m_type));
    }
    else if (passCreateInfo.m_type == RenderPassType::Compute)
    {
      D3D12ScopedComputePass computePass = D3D12ScopedComputePass(passCount, m_computePasses.GetSize(), mainAllocator,
        log_D3D12RenderSystem);

      computePass.Create(m_device,
        passCreateInfo,
        renderPassRequirements.m_targets,
        m_targetImages,
        m_targetBuffers,
        m_descriptorSlots,
        m_descriptorSetTable,
        m_shaders);

      m_computePasses.PushBack(computePass);
      m_renderSequence.PushBack(std::make_pair(m_computePasses.GetSize() - 1, passCreateInfo.m_type));
    }

    ++passCount;
  }
}

String D3D12Renderer::GetRenderingAPI() const {
  return "D3D12";
}

DrawablePool& D3D12Renderer::CreateDrawablePool(const DrawablePoolCreateInfo& createInfo) {
  D3D12DrawablePool pool = D3D12DrawablePool(m_device,
                                             createInfo,
                                             m_descriptorCount,
                                             m_descriptorSlots,
                                             m_shaders,
                                             m_renderPasses,
                                             m_targetBuffers,
                                             m_mainGraphicsCommandQueue,
                                             m_drawPoolAllocator,
                                             m_initAllocator,
                                             log_D3D12RenderSystem);

  m_drawablePools.PushBack(std::move(pool));

  return m_drawablePools.Last();
}

ComputePool& D3D12Renderer::CreateComputePool(const ComputePoolCreateInfo& createInfo) {
  D3D12ComputePool pool = D3D12ComputePool(m_device,
    createInfo,
    m_descriptorCount,
    m_descriptorSlots,
    m_shaders,
    m_computePasses,
    m_mainComputeCommandQueue,
    m_mainGraphicsCommandQueue,
    m_drawPoolAllocator,
    m_initAllocator,
    log_D3D12RenderSystem);

  m_computePools.PushBack(std::move(pool));

  return m_computePools.Last();
}

void D3D12Renderer::Submit() {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "Submitting Renderer");

  HEAP_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 8192);

  const bool haveSubmits = m_renderTargetUpdates.GetSize() > 0 || m_bufferTargetUpdates.GetSize() > 0;

  if (haveSubmits)
  {
    auto oneTimeSubmitBuffer = D3D12ScopedCommandBuffer(m_device, D3D12_COMMAND_LIST_TYPE_DIRECT, log_D3D12RenderSystem);
    oneTimeSubmitBuffer.CreateGraphicsCommandList(m_device, nullptr, log_D3D12RenderSystem);
    auto oneTimeCommandList = oneTimeSubmitBuffer.GetGraphicsCommandList();

    LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "Updating Render Targets with Initial Data");

    for (const auto& renderTargetUpdate : m_renderTargetUpdates)
    {
      // TODO(vasumahesh1):[STATES]: Get current state and don't hardcode
      auto& targetImage = m_targetImages[renderTargetUpdate.m_binding];
      targetImage.Transition(oneTimeCommandList, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST, log_D3D12RenderSystem);
      targetImage.CopyFromBuffer(m_device, oneTimeCommandList, m_stagingBuffer, renderTargetUpdate.m_offset);
      targetImage.Transition(oneTimeCommandList, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON, log_D3D12RenderSystem);
    }

    LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "Updating Buffer Targets with Initial Data");

    for (const auto& bufferTargetUpdate : m_bufferTargetUpdates)
    {
      auto& targetBuffer = m_targetBuffers[bufferTargetUpdate.m_binding];
      targetBuffer.Transition(oneTimeCommandList, D3D12_RESOURCE_STATE_COPY_DEST, log_D3D12RenderSystem);
      D3D12Core::CopyBuffer(oneTimeCommandList, targetBuffer, 0, m_stagingBuffer, bufferTargetUpdate.m_offset, bufferTargetUpdate.m_byteSize);
      targetBuffer.Transition(oneTimeCommandList, D3D12_RESOURCE_STATE_COMMON, log_D3D12RenderSystem);
    }

    oneTimeCommandList->Close();
    oneTimeSubmitBuffer.Execute(m_device, m_mainGraphicsCommandQueue.Get(), log_D3D12RenderSystem);
    oneTimeSubmitBuffer.WaitForComplete(m_mainGraphicsCommandQueue.Get(), log_D3D12RenderSystem);
  }

  for (auto& drawablePool : m_drawablePools) {
    drawablePool.Submit();
  }

  Vector<Vector<D3D12RenderPassRecordEntry>> renderPassEntries(m_renderPasses.GetSize(), allocatorTemporary);
  for (U32 idx = 0; idx < m_renderPasses.GetSize(); ++idx) {
    Vector<D3D12RenderPassRecordEntry> poolEntries(m_drawablePools.GetSize(), allocatorTemporary);
    renderPassEntries.PushBack(poolEntries);
  }

  U32 poolIdx = 0;
  for (auto& drawablePool : m_drawablePools) {
    Vector<std::pair<U32, D3D12RenderPassRecordEntry>> poolEntries(allocatorTemporary);
    drawablePool.GetRecordEntries(poolEntries);

    for (const auto& bufferPair : poolEntries) {
      renderPassEntries[bufferPair.first].PushBack(bufferPair.second);
      renderPassEntries[bufferPair.first].Last().m_poolIdx = poolIdx;
    }

    ++poolIdx;
  }

  for (U32 idx = 0; idx < m_renderPasses.GetSize(); ++idx) {

    const auto& renderPass = m_renderPasses[idx];

    const bool isLast = idx == m_renderPasses.GetSize() - 1;

    const U32 commandBuffers = renderPass.GetCommandBufferCount();

    for (U32 cIdx      = 0; cIdx < commandBuffers; ++cIdx) {
      auto commandList = renderPass.GetPrimaryGraphicsCommandList(cIdx);

      if (isLast) {
        renderPass.RecordImageAcquireBarrier(commandList, cIdx);
      }

      renderPass.RecordResourceBarriersForOutputsStart(commandList);
      renderPass.RecordResourceBarriersForInputsStart(commandList);

      commandList->RSSetViewports(1, &m_viewport);
      commandList->RSSetScissorRects(1, &m_scissorRect);

      renderPass.SetRenderTargets(commandList, cIdx, m_rtvDescriptorSize);

      const auto& renderPassRecords = renderPassEntries[idx];

      commandList->SetGraphicsRootSignature(renderPass.GetRootSignature());

      for (auto& recordEntry : renderPassRecords) {
        const auto& targetPool = m_drawablePools[recordEntry.m_poolIdx];

        commandList->SetPipelineState(recordEntry.m_pso);

        const auto& allHeaps = targetPool.GetAllDescriptorHeaps();
        commandList->SetDescriptorHeaps(UINT(allHeaps.GetSize()), allHeaps.Data());

        commandList->ExecuteBundle(recordEntry.m_bundle);
      }

      renderPass.RecordResourceBarriersForOutputsEnd(commandList);
      renderPass.RecordResourceBarriersForInputsEnd(commandList);

      if (isLast) {
        renderPass.RecordPresentBarrier(commandList, cIdx);
      }

      commandList->Close();
    }
  }


  // COMPUTE SUBMIT

  for (auto& computePool : m_computePools) {
    computePool.Submit();
  }

  // Begin Recording - Open all command buffers
  for (U32 idx = 0; idx < m_computePasses.GetSize(); ++idx) {
    auto& computePass = m_computePasses[idx];

    LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "Begin Recording Compute Pass: %d", idx);

    const auto commandList = computePass.GetPrimaryComputeCommandList(0);

    computePass.RecordResourceBarriersForOutputsStart(commandList);
    computePass.RecordResourceBarriersForInputsStart(commandList);
  }

  // Record Pool Wise - Pass Wise data
  for (auto& computePool : m_computePools) {
    computePool.Record();
  }

  // Close Recording
  for (U32 idx = 0; idx < m_computePasses.GetSize(); ++idx) {
    auto& computePass = m_computePasses[idx];

    const auto commandList = computePass.GetPrimaryComputeCommandList(0);

    computePass.RecordResourceBarriersForOutputsEnd(commandList);
    computePass.RecordResourceBarriersForInputsEnd(commandList);
    commandList->Close();

    LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "End Recording Compute Pass: %d", idx);
  }

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "Submit Complete - All Render Passes recorded");
}

void D3D12Renderer::RenderFrame() {
  EnterRenderFrame();

  const auto& currentFrame = GetCurrentFrame();

  U32 count = 0;
  for(const auto& pass : m_renderSequence)
  {
    if (count == m_renderSequence.GetSize() - 1)
    {
      // Don't do last render pass
      break;
    }

    const auto idx = pass.first;

    if (pass.second == RenderPassType::Graphics)
    {
      std::array<ID3D12CommandList*, 1> list = { m_renderPasses[idx].GetPrimaryGraphicsCommandList(0) };
      m_mainGraphicsCommandQueue->ExecuteCommandLists(UINT(list.size()), list.data());
      m_renderPasses[idx].WaitForGPU(m_mainGraphicsCommandQueue.Get());
    }
    else if (pass.second == RenderPassType::Compute)
    {
      std::array<ID3D12CommandList*, 1> list = { m_computePasses[idx].GetPrimaryComputeCommandList(0) };
      m_mainComputeCommandQueue->ExecuteCommandLists(UINT(list.size()), list.data());
      m_computePasses[idx].WaitForGPU(m_mainComputeCommandQueue.Get());
    }

    ++count;
  }

  std::array<ID3D12CommandList*, 1> swapChainList = {m_renderPasses.Last().GetPrimaryGraphicsCommandList(currentFrame)};
  m_mainGraphicsCommandQueue->ExecuteCommandLists(UINT(swapChainList.size()), swapChainList.data()); // NOLINT

  // Present and update the frame index for the next frame.
  VERIFY_D3D_OP(log_D3D12RenderSystem, m_swapChain.RealComPtr()->Present(1, 0), "Present Failed");

  m_renderPasses.Last().WaitForGPU(m_mainGraphicsCommandQueue.Get());

  m_perFrameAllocator.Reset();
  ExitRenderFrame();
}

void D3D12Renderer::SnapshotFrame(const String& exportPath) const {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "Exporting Screenshot: %s", exportPath.c_str());
  UNUSED(exportPath);
}

void D3D12Renderer::AddShader(const ShaderCreateInfo& info) {
  const String fullPath = "Shaders/" + D3D12Renderer::GetRenderingAPI() + "/" + info.m_shaderFileName;

  D3D12ScopedShader shader = D3D12ScopedShader(fullPath, m_initAllocator, log_D3D12RenderSystem);
  m_shaders.PushBack(std::move(shader));

  m_shaders.Last().SetStage(info.m_stage);
}

void D3D12Renderer::BindRenderTarget(U32 renderTargetId, const TextureDesc& desc, const U8* buffer) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL,
    "D3D12 Render Target: Updating Render Target: %d of Size: %d bytes", renderTargetId, desc.m_size);

  const U32 size = desc.m_size;

  const U32 textureWidthBytes = desc.m_bounds.m_width * GetFormatSize(desc.m_format);

  // TODO(vasumahesh1):[INPUT]: Could be an issue with sizeof(float)
  const U32 offset = m_stagingBuffer.AppendTextureData(buffer, size, 512, textureWidthBytes, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT, log_D3D12RenderSystem);

  TextureBufferInfo info = TextureBufferInfo();
  info.m_byteSize        = size;
  info.m_offset          = offset;
  info.m_desc            = desc;
  info.m_binding         = renderTargetId;
  info.m_set             = 0;

  m_renderTargetUpdates.PushBack(info);
}

void D3D12Renderer::BindBufferTarget(U32 bufferTargetId, const U8* buffer) {
  const U32 size = m_targetBuffers[bufferTargetId].GetSize();

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL,
    "D3D12 Render Target: Updating Buffer Target: %d of Size: %d bytes", bufferTargetId, size);

  // TODO(vasumahesh1):[INPUT]: Could be an issue with sizeof(float)
  const U32 offset = m_stagingBuffer.AppendData(buffer, size, sizeof(float), log_D3D12RenderSystem);

  BufferTargetInfo info = BufferTargetInfo();
  info.m_byteSize        = size;
  info.m_offset          = offset;
  info.m_binding         = bufferTargetId;
  info.m_set             = 0;

  m_bufferTargetUpdates.PushBack(info);
}

} // namespace D3D12
} // namespace Azura
