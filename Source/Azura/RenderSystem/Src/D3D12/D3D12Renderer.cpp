#include "D3D12/D3D12Renderer.h"
#include "D3D12/D3D12Macros.h"

#include "D3D12/d3dx12.h"

using namespace Microsoft::WRL; // NOLINT

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
    m_drawablePools(renderPassRequirements.m_maxPools, drawAllocator),
    m_shaders(shaderRequirements.m_shaders.GetSize(), mainAllocator),
    m_primaryCommandBuffers(swapChainRequirements.m_framesInFlight, mainAllocator) {
  UNUSED(renderPassRequirements);
  UNUSED(shaderRequirements);

  const auto viewportDimensions = window.GetViewport();
  m_viewport                    = CD3DX12_VIEWPORT(0.0f, 0.0f, viewportDimensions.m_width, viewportDimensions.m_height);
  m_scissorRect                 = CD3DX12_RECT(0, 0, static_cast<LONG>(viewportDimensions.m_width),
                                               static_cast<LONG>(viewportDimensions.m_height));

  UINT dxgiFactoryFlags = 0;

#ifdef BUILD_DEBUG
  ComPtr<ID3D12Debug> debugController;
  ComPtr<ID3D12Debug1> debugController1;
  if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
    debugController->EnableDebugLayer();

    debugController->QueryInterface(IID_PPV_ARGS(&debugController1));
    debugController1->SetEnableGPUBasedValidation(true); // NOLINT

    // Enable additional debug layers.
    dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
  }
#endif

  ComPtr<IDXGIFactory4> factory;
  VERIFY_D3D_OP(log_D3D12RenderSystem, CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)),
    "Failed to create DXGI Factory");

  ComPtr<IDXGIAdapter1> hardwareAdapter;
  D3D12Core::GetHardwareAdapter(factory.Get(), &hardwareAdapter);

  VERIFY_D3D_OP(log_D3D12RenderSystem, D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&
    m_device)), "Failed to create D3D12 Device");

  // Describe and create the command queue.
  D3D12_COMMAND_QUEUE_DESC queueDesc = {};
  queueDesc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
  queueDesc.Type                     = D3D12_COMMAND_LIST_TYPE_DIRECT;

  VERIFY_D3D_OP(log_D3D12RenderSystem, m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)),
    "Failed to create command Queue");

  auto swapChain = D3D12Core::CreateSwapChain(factory, m_commandQueue, m_window.get().GetHandle(),
                                              swapChainRequirements, log_D3D12RenderSystem);
  VERIFY_D3D_OP(log_D3D12RenderSystem, swapChain.As(&m_swapChain), "Swapchain typecast failed");

  SetCurrentFrame(m_swapChain->GetCurrentBackBufferIndex());

  for (const auto& shaderCreateInfo : shaderRequirements.m_shaders) {
    D3D12Renderer::AddShader(shaderCreateInfo);
  }

  // TODO(vasumahesh1):[RENDER-PASS]: We might need to create multiple RTV per pass
  // Describe and create a render target view (RTV) descriptor heap.
  D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
  rtvHeapDesc.NumDescriptors             = swapChainRequirements.m_framesInFlight;
  rtvHeapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
  rtvHeapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  VERIFY_D3D_OP(log_D3D12RenderSystem, m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)),
    "Failed to create descriptor heaps for RTV");

  m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

  // Create Frame buffers
  CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

  // Create a RTV for each frame.
  for (U32 n = 0; n < swapChainRequirements.m_framesInFlight; n++) {
    auto& renderTarget = m_renderTargets[n]; // NOLINT
    VERIFY_D3D_OP(log_D3D12RenderSystem, m_swapChain->GetBuffer(n, IID_PPV_ARGS(&renderTarget)),
      "Failed to get swapchain buffer");
    m_device->CreateRenderTargetView(renderTarget.Get(), nullptr, rtvHandle);
    rtvHandle.Offset(1, m_rtvDescriptorSize);
  }

  for (UINT n                                 = 0; n < swapChainRequirements.m_framesInFlight; n++) {
    D3D12ScopedCommandBuffer primaryCmdBuffer = D3D12ScopedCommandBuffer(m_device, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                                         log_D3D12RenderSystem);
    primaryCmdBuffer.CreateGraphicsCommandList(m_device, nullptr, log_D3D12RenderSystem);
    m_primaryCommandBuffers.PushBack(primaryCmdBuffer);
  }

  VERIFY_D3D_OP(log_D3D12RenderSystem, m_device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)
  ), "Failed to create fence");
  m_fenceValue++;

  // Create an event handle to use for frame synchronization.
  m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
  if (m_fenceEvent == nullptr) {
    VERIFY_D3D_OP(log_D3D12RenderSystem, HRESULT_FROM_WIN32(GetLastError()), "Fence Event Null");
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
                                             m_drawPoolAllocator,
                                             m_initAllocator,
                                             log_D3D12RenderSystem);

  m_drawablePools.PushBack(std::move(pool));

  return m_drawablePools.Last();
}

void D3D12Renderer::Submit() {
  for (auto& drawablePool : m_drawablePools) {
    drawablePool.Submit();
  }

  U32 idx = 0;
  for (auto& primaryBuffer : m_primaryCommandBuffers) {
    auto commandList = primaryBuffer.GetGraphicsCommandList();

    auto& renderTarget = m_renderTargets[idx]; // NOLINT

    CD3DX12_RESOURCE_BARRIER backBufferBarrierStart = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget.Get(),
                                                                                           D3D12_RESOURCE_STATE_PRESENT,
                                                                                           D3D12_RESOURCE_STATE_RENDER_TARGET);
    CD3DX12_RESOURCE_BARRIER backBufferBarrierEnd = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget.Get(),
                                                                                         D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                                                         D3D12_RESOURCE_STATE_PRESENT);

    commandList->RSSetViewports(1, &m_viewport);
    commandList->RSSetScissorRects(1, &m_scissorRect);

    commandList->ResourceBarrier(1, &backBufferBarrierStart);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), idx, m_rtvDescriptorSize);
    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    const float clearColor[] = {0.2f, 0.2f, 0.2f, 1.0f};
    commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr); // NOLINT

    for (auto& drawablePool : m_drawablePools) {
      commandList->SetPipelineState(drawablePool.GetPipelineState());
      commandList->SetGraphicsRootSignature(drawablePool.GetRootSignature());

      ID3D12DescriptorHeap* ppHeaps[] = {drawablePool.GetDescriptorHeap()};
      commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps); // NOLINT

      commandList->ExecuteBundle(drawablePool.GetSecondaryCommandList());
    }

    commandList->ResourceBarrier(1, &backBufferBarrierEnd);

    commandList->Close();
    ++idx;
  }
}

void D3D12Renderer::RenderFrame() {
  EnterRenderFrame();

  const auto& currentFrame = GetCurrentFrame();

  ID3D12CommandList* ppCommandLists[] = {m_primaryCommandBuffers[currentFrame].GetGraphicsCommandList()};
  m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists); // NOLINT

  // Present and update the frame index for the next frame.
  VERIFY_D3D_OP(log_D3D12RenderSystem, m_swapChain->Present(1, 0), "Present Failed");

  WaitForGPU();

  ExitRenderFrame();
}

void D3D12Renderer::WaitForGPU()
{
  const U32 fence = m_fenceValue;
  VERIFY_D3D_OP(log_D3D12RenderSystem, m_commandQueue->Signal(m_fence.Get(), m_fenceValue), "Fence wait failed");
  m_fenceValue++;

  // Wait until the previous frame is finished.
  if (m_fence->GetCompletedValue() < fence)
  {
    VERIFY_D3D_OP(log_D3D12RenderSystem, m_fence->SetEventOnCompletion(fence, m_fenceEvent), "Failed to set event completion on Fence");
    WaitForSingleObject(m_fenceEvent, INFINITE);
  }
}

void D3D12Renderer::SnapshotFrame(const String& exportPath) const {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "Exporting Screenshot: %s", exportPath.c_str());
}

void D3D12Renderer::AddShader(const ShaderCreateInfo& info) {
  const String fullPath = "Shaders/" + D3D12Renderer::GetRenderingAPI() + "/" + info.m_shaderFileName;

  D3D12ScopedShader shader = D3D12ScopedShader(fullPath, m_initAllocator, log_D3D12RenderSystem);
  m_shaders.PushBack(std::move(shader));

  m_shaders.Last().SetStage(info.m_stage);
}
} // namespace D3D12
} // namespace Azura
