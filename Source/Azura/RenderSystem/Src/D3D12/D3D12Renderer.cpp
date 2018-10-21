#include "D3D12/D3D12Renderer.h"
#include "D3D12/D3D12Macros.h"

#include "D3D12/d3dx12.h"
#include "Memory/MemoryFactory.h"

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
    m_renderTargetImages(renderPassRequirements.m_targets.GetSize(), mainAllocator),
    m_renderPasses(renderPassRequirements.m_passSequence.GetSize(), mainAllocator),
    m_drawablePools(renderPassRequirements.m_maxPools, drawAllocator),
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

  VERIFY_D3D_OP(log_D3D12RenderSystem, m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)),
    "Failed to create command Queue");

  m_swapChain.Create(factory, m_commandQueue, m_window.get(), swapChainRequirements, log_D3D12RenderSystem);

  SetCurrentFrame(m_swapChain.RealComPtr()->GetCurrentBackBufferIndex());

  for (const auto& shaderCreateInfo : shaderRequirements.m_shaders) {
    D3D12Renderer::AddShader(shaderCreateInfo);
  }

  for (const auto& renderTarget : renderPassRequirements.m_targets) {
    TextureDesc desc = {};
    desc.m_format    = renderTarget.m_format;
    desc.m_bounds    = Bounds3D{swapChainRequirements.m_extent.m_width, swapChainRequirements.m_extent.m_height, 1};

    LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "Creating Attachment Input at: %d for %s", m_renderTargetImages.
      GetSize(), ToString(renderTarget.m_format).c_str());

    D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;
    if (HasDepthOrStencilComponent(renderTarget.m_format)) {
      resourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    }

    D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    if (HasDepthComponent(desc.m_format) || HasStencilComponent(desc.m_format)) {
      resourceFlags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    }

    m_renderTargetImages.PushBack(D3D12ScopedImage());
    m_renderTargetImages.Last().Create(m_device, resourceState, resourceFlags, desc, log_D3D12RenderSystem);
  }

  TextureDesc desc = {};
  desc.m_format    = swapChainRequirements.m_depthFormat;
  desc.m_bounds    = Bounds3D{swapChainRequirements.m_extent.m_width, swapChainRequirements.m_extent.m_height, 1};

  const D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
  const D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
  m_depthTexture.Create(m_device, resourceState, resourceFlags, desc, log_D3D12RenderSystem);

  m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
  m_dsvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

  U32 passCount = 0;
  for (const auto& passCreateInfo : renderPassRequirements.m_passSequence) {
    D3D12ScopedRenderPass renderPass = D3D12ScopedRenderPass(m_renderPasses.GetSize(), m_swapChain, mainAllocator,
                                                             log_D3D12RenderSystem);

    if (passCount != (renderPassRequirements.m_passSequence.GetSize() - 1)) {
      renderPass.Create(m_device,
                        passCreateInfo,
                        renderPassRequirements.m_targets,
                        m_renderTargetImages,
                        m_descriptorSlots,
                        m_descriptorSetTable,
                        m_shaders,
                        m_rtvDescriptorSize, m_dsvDescriptorSize);
    } else {
      renderPass.CreateForSwapChain(m_device,
                                    passCreateInfo,
                                    renderPassRequirements.m_targets,
                                    m_renderTargetImages,
                                    m_depthTexture,
                                    m_descriptorSlots,
                                    m_descriptorSetTable,
                                    m_shaders,
                                    m_rtvDescriptorSize, m_dsvDescriptorSize);
    }

    m_renderPasses.PushBack(renderPass);
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
                                             m_drawPoolAllocator,
                                             m_initAllocator,
                                             log_D3D12RenderSystem);

  m_drawablePools.PushBack(std::move(pool));

  return m_drawablePools.Last();
}

void D3D12Renderer::Submit() {
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 4096);

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

      renderPass.RecordResourceBarriersForReadingInputs(commandList);

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

      renderPass.RecordResourceBarriersForWritingInputs(commandList);

      if (isLast) {
        renderPass.RecordPresentBarrier(commandList, cIdx);
      }

      commandList->Close();
    }
  }
}

void D3D12Renderer::RenderFrame() {
  EnterRenderFrame();

  // TODO(vasumahesh1): Change
  SmallVector<ID3D12CommandList*, 10> pCommandLists;

  const auto& currentFrame = GetCurrentFrame();

  for (U32 idx = 0; idx < m_renderPasses.GetSize() - 1; ++idx) {
    pCommandLists.push_back(m_renderPasses[idx].GetPrimaryGraphicsCommandList(0));
  }

  pCommandLists.push_back(m_renderPasses.Last().GetPrimaryGraphicsCommandList(currentFrame));

  m_commandQueue->ExecuteCommandLists(UINT(pCommandLists.size()), pCommandLists.data()); // NOLINT

  // Present and update the frame index for the next frame.
  VERIFY_D3D_OP(log_D3D12RenderSystem, m_swapChain.RealComPtr()->Present(1, 0), "Present Failed");

  WaitForGPU();

  m_perFrameAllocator.Reset();

  ExitRenderFrame();
}

void D3D12Renderer::WaitForGPU() {
  m_renderPasses.Last().WaitForGPU(m_commandQueue.Get());
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
} // namespace D3D12
} // namespace Azura
