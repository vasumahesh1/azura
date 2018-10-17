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
    m_renderTargets(swapChainRequirements.m_framesInFlight, mainAllocator),
    m_drawablePools(renderPassRequirements.m_maxPools, drawAllocator),
    m_shaders(shaderRequirements.m_shaders.GetSize(), mainAllocator) {
  UNUSED(renderPassRequirements);
  UNUSED(shaderRequirements);

  UINT dxgiFactoryFlags = 0;

#ifdef BUILD_DEBUG
  ComPtr<ID3D12Debug> debugController;
  if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
  {
    debugController->EnableDebugLayer();

    // Enable additional debug layers.
    dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
  }
#endif

  ComPtr<IDXGIFactory4> factory;
  VERIFY_D3D_OP(log_D3D12RenderSystem, CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)), "Failed to create DXGI Factory");

  ComPtr<IDXGIAdapter1> hardwareAdapter;
  D3D12Core::GetHardwareAdapter(factory.Get(), &hardwareAdapter);

  VERIFY_D3D_OP(log_D3D12RenderSystem, D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)), "Failed to create D3D12 Device");

  // Describe and create the command queue.
  D3D12_COMMAND_QUEUE_DESC queueDesc = {};
  queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
  queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

  VERIFY_D3D_OP(log_D3D12RenderSystem, m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)), "Failed to create command Queue");

  auto swapChain = D3D12Core::CreateSwapChain(factory, m_commandQueue, m_window.get().GetHandle(), swapChainRequirements, log_D3D12RenderSystem);
  VERIFY_D3D_OP(log_D3D12RenderSystem, swapChain.As(&m_swapChain), "Swapchain typecast failed");

  SetCurrentFrame(m_swapChain->GetCurrentBackBufferIndex());

  for (const auto& shaderCreateInfo : shaderRequirements.m_shaders) {
    D3D12Renderer::AddShader(shaderCreateInfo);
  }

  // TODO(vasumahesh1):[RENDER-PASS]: We might need to create multiple RTV per pass
  // Describe and create a render target view (RTV) descriptor heap.
  D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
  rtvHeapDesc.NumDescriptors = swapChainRequirements.m_framesInFlight;
  rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
  rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  VERIFY_D3D_OP(log_D3D12RenderSystem, m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)), "Failed to create descriptor heaps for RTV");

  m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

  // Create Frame buffers
  CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

  // Create a RTV for each frame.
  for (UINT n = 0; n < swapChainRequirements.m_framesInFlight; n++)
  {
    VERIFY_D3D_OP(log_D3D12RenderSystem, m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])), "Failed to get swapchain buffer");
    m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
    rtvHandle.Offset(1, m_rtvDescriptorSize);
  }

  VERIFY_D3D_OP(log_D3D12RenderSystem, m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)), "Failed to create command allocator");
}

String D3D12Renderer::GetRenderingAPI() const {
  return "D3D12";
}

DrawablePool& D3D12Renderer::CreateDrawablePool(const DrawablePoolCreateInfo& createInfo) {
  D3D12DrawablePool pool = D3D12DrawablePool(m_device, createInfo, m_descriptorCount, m_drawPoolAllocator, log_D3D12RenderSystem);
  m_drawablePools.PushBack(std::move(pool));

  return m_drawablePools.Last();
}

void D3D12Renderer::Submit() {
}

void D3D12Renderer::RenderFrame() {
}

void D3D12Renderer::SnapshotFrame(const String& exportPath) const {
}

void D3D12Renderer::AddShader(const ShaderCreateInfo& info) {
  const String fullPath = "Shaders/" + D3D12Renderer::GetRenderingAPI() + "/" + info.m_shaderFileName;

  D3D12ScopedShader shader = D3D12ScopedShader(fullPath, m_initAllocator, log_D3D12RenderSystem);
  m_shaders.PushBack(std::move(shader));

  m_shaders.Last().SetStage(info.m_stage);
}
} // namespace D3D12
} // namespace Azura
