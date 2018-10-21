#pragma once

#include "Generic/Window.h"

#include "D3D12/D3D12Core.h"

namespace Azura {
struct SwapChainRequirements;
class Log;
}

namespace Azura {
namespace D3D12 {

class D3D12ScopedSwapChain
{
public:

  void Create(const Microsoft::WRL::ComPtr<IDXGIFactory4>& factory,
    const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& commandQueue,
    const Window& windowHandle,
    const SwapChainRequirements& swapChainRequirements,
    const Log& log_D3D12RenderSystem);

  const Microsoft::WRL::ComPtr<IDXGISwapChain3>& RealComPtr() const;
  IDXGISwapChain3* Real() const;

  RawStorageFormat GetFormat() const;
  RawStorageFormat GetDepthFormat() const;

private:
  RawStorageFormat m_format{};
  RawStorageFormat m_depthFormat{};

  Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
  Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTargets[GLOBAL_INFLIGHT_FRAMES];

  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
};

} // namespace D3D12
} // namespace Azura
