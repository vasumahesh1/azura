#pragma once
#include "Generic/Renderer.h"
#include "Generic/Window.h"

#include "Log/Log.h"
#include "D3D12/D3D12DrawablePool.h"
#include "D3D12/D3D12Core.h"


namespace Azura {
namespace D3D12 {

class D3D12Renderer : public Renderer {

public:
  D3D12Renderer(const ApplicationInfo& appInfo,
                const DeviceRequirements& deviceRequirements,
                const ApplicationRequirements& appRequirements,
                const SwapChainRequirements& swapChainRequirements,
                const RenderPassRequirements& renderPassRequirements,
                const DescriptorRequirements& descriptorRequirements,
                const ShaderRequirements& shaderRequirements,
                Memory::Allocator& mainAllocator,
                Memory::Allocator& drawAllocator,
                Window& window);

  String GetRenderingAPI() const override;
  DrawablePool& CreateDrawablePool(const DrawablePoolCreateInfo& createInfo) override;
  void Submit() override;
  void RenderFrame() override;
  void SnapshotFrame(const String& exportPath) const override;

private:
  void AddShader(const ShaderCreateInfo& info) override;

  Log log_D3D12RenderSystem;

  Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
  Microsoft::WRL::ComPtr<ID3D12Device> m_device;

  Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
  Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;

  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
  Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
  UINT m_rtvDescriptorSize;

  Containers::Vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_renderTargets;

  Containers::Vector<D3D12DrawablePool> m_drawablePools;
};

} // namespace D3D12
} // namespace Azura
