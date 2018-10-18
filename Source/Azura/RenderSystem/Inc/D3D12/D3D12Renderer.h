#pragma once
#include "Generic/Renderer.h"
#include "Generic/Window.h"

#include "Log/Log.h"
#include "D3D12/D3D12DrawablePool.h"
#include "D3D12/D3D12Core.h"
#include "D3D12/D3D12ScopedShader.h"

#include "Memory/MonotonicAllocator.h"
#include "Memory/HeapMemoryBuffer.h"
#include "D3D12ScopedCommandBuffer.h"


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

  Memory::HeapMemoryBuffer m_perFrameBuffer;
  Memory::MonotonicAllocator m_perFrameAllocator;

  Memory::HeapMemoryBuffer m_initBuffer;
  Memory::MonotonicAllocator m_initAllocator;

  Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
  Microsoft::WRL::ComPtr<ID3D12Device> m_device;

  Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
  Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;

  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
  UINT m_rtvDescriptorSize;

  CD3DX12_VIEWPORT m_viewport;
  CD3DX12_RECT m_scissorRect;

  HANDLE m_fenceEvent;
  U32 m_fenceValue{0};
  Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;

  Containers::Vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_renderTargets;

  Containers::Vector<D3D12DrawablePool> m_drawablePools;
  Containers::Vector<D3D12ScopedShader> m_shaders;
  Containers::Vector<D3D12ScopedCommandBuffer> m_primaryCommandBuffers;

};

} // namespace D3D12
} // namespace Azura
