#pragma once

#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>

#include <wrl.h>
#include <shellapi.h>

#include "Generic/GenericTypes.h"

namespace Azura {
namespace D3D12 {
class D3D12ScopedBuffer;
}

class Log;
}

namespace Azura {
namespace D3D12 {

namespace D3D12Core {
Microsoft::WRL::ComPtr<IDXGISwapChain1> CreateSwapChain(const Microsoft::WRL::ComPtr<IDXGIFactory4>& factory,
                                                        const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& commandQueue,
                                                        const void* windowHandle,
                                                        const SwapChainRequirements& swapChainRequirements,
                                                        const Log& log_D3D12RenderSystem);

void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter, const Log& log_D3D12RenderSystem);

void CopyBuffer(ID3D12GraphicsCommandList* commandList,
                const D3D12ScopedBuffer& dest,
                const D3D12ScopedBuffer& src,
                UINT64 size);

void CopyBuffer(ID3D12GraphicsCommandList* commandList,
                const D3D12ScopedBuffer& dest,
                UINT64 dstOffset,
                const D3D12ScopedBuffer& src,
                UINT64 srcOffset,
                UINT64 size);
}

} // namespace D3D12
} // namespace Azura
