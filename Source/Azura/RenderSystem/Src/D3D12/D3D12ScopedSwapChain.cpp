#include "D3D12/D3D12ScopedSwapChain.h"
#include "D3D12/D3D12Macros.h"
#include "Log/Log.h"
#include "D3D12/d3dx12.h"


namespace Azura {
namespace D3D12 {

void D3D12ScopedSwapChain::Create(
  const Microsoft::WRL::ComPtr<IDXGIFactory4>& factory,
  const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& commandQueue,
  const Window& windowHandle,
  const SwapChainRequirements& swapChainRequirements,
  const Log& log_D3D12RenderSystem) {

  m_format = swapChainRequirements.m_format;

  auto swapChain = D3D12Core::CreateSwapChain(factory, commandQueue, windowHandle.GetHandle(),
                                              swapChainRequirements, log_D3D12RenderSystem);
  VERIFY_D3D_OP(log_D3D12RenderSystem, swapChain.As(&m_swapChain), "Swapchain typecast failed");
}

const Microsoft::WRL::ComPtr<IDXGISwapChain3>& D3D12ScopedSwapChain::RealComPtr() const {
  return m_swapChain;
}

IDXGISwapChain3* D3D12ScopedSwapChain::Real() const {
  return m_swapChain.Get();
}

RawStorageFormat D3D12ScopedSwapChain::GetFormat() const {
  return m_format;
}
} // namespace D3D12
} // namespace Azura
