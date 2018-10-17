#include "D3D12/D3D12Core.h"

#include "Log/Log.h"
#include "D3D12/D3D12Macros.h"

using namespace Microsoft::WRL; // NOLINT

namespace Azura {
namespace D3D12 {

ComPtr<IDXGISwapChain1> D3D12Core::CreateSwapChain(const ComPtr<IDXGIFactory4>& factory,
                                                   const ComPtr<ID3D12CommandQueue>& commandQueue,
                                                   const void* windowHandle,
                                                   const SwapChainRequirements& swapChainRequirements,
                                                   const Log& log_D3D12RenderSystem) {

  // Describe and create the swap chain.
  DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
  swapChainDesc.BufferCount           = swapChainRequirements.m_framesInFlight;
  swapChainDesc.Width                 = swapChainRequirements.m_extent.m_width;
  swapChainDesc.Height                = swapChainRequirements.m_extent.m_height;
  swapChainDesc.Format                = DXGI_FORMAT_R8G8B8A8_UNORM;
  swapChainDesc.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapChainDesc.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;
  swapChainDesc.SampleDesc.Count      = 1;

  ComPtr<IDXGISwapChain1> swapChain;

  VERIFY_D3D_OP(log_D3D12RenderSystem, factory->CreateSwapChainForHwnd(
    commandQueue.Get(),
    *reinterpret_cast<const HWND*>(windowHandle), // NOLINT
    &swapChainDesc,
    nullptr,
    nullptr,
    &swapChain
  ), "Failed to create D3D12 Swapchain");

  return swapChain;
}

void D3D12Core::GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter) {
  ComPtr<IDXGIAdapter1> adapter;
  *ppAdapter = nullptr;

  for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
  {
    DXGI_ADAPTER_DESC1 desc;
    adapter->GetDesc1(&desc);

    if ((desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0u)
    {
      // Don't select the Basic Render Driver adapter.
      continue;
    }

    // Check to see if the adapter supports Direct3D 12, but don't create the
    // actual device yet.
    if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
    {
      break;
    }
  }

  *ppAdapter = adapter.Detach();
}

} // namespace D3D12
} // namespace Azura
