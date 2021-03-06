#pragma once
#include "Generic/Renderer.h"
#include "Generic/Window.h"

#include "Log/Log.h"
#include "D3D12/D3D12DrawablePool.h"
#include "D3D12/D3D12Core.h"
#include "D3D12/D3D12ScopedShader.h"

#include "Memory/MonotonicAllocator.h"
#include "Memory/HeapMemoryBuffer.h"
#include "D3D12/D3D12ScopedCommandBuffer.h"
#include "D3D12/D3D12ScopedSwapChain.h"
#include "D3D12/D3D12ScopedRenderPass.h"
#include "D3D12/D3D12ScopedComputePass.h"
#include "D3D12/D3D12ComputePool.h"


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
  ComputePool& CreateComputePool(const ComputePoolCreateInfo& createInfo) override;

  void Submit() override;
  void RenderFrame() override;

  void SnapshotFrame(const String& exportPath) const override;

  void BindRenderTarget(U32 renderTargetId, const TextureDesc& desc, const U8* buffer) override;
  void BindBufferTarget(U32 bufferTargetId, const U8* buffer) override;

private:
  void AddShader(const ShaderCreateInfo& info) override;

  Log log_D3D12RenderSystem;

  Memory::HeapMemoryBuffer m_perFrameBuffer;
  Memory::MonotonicAllocator m_perFrameAllocator;

  Memory::HeapMemoryBuffer m_initBuffer;
  Memory::MonotonicAllocator m_initAllocator;

  D3D12ScopedSwapChain m_swapChain;
  Microsoft::WRL::ComPtr<ID3D12Device> m_device;

  Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
  Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_mainGraphicsCommandQueue;
  Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_mainComputeCommandQueue;

  UINT m_rtvDescriptorSize;
  UINT m_dsvDescriptorSize;

  CD3DX12_VIEWPORT m_viewport;
  CD3DX12_RECT m_scissorRect;

  D3D12ScopedImage m_depthTexture{};

  D3D12ScopedBuffer m_stagingBuffer;

  Containers::Vector<TextureBufferInfo> m_renderTargetUpdates;
  Containers::Vector<BufferTargetInfo> m_bufferTargetUpdates;

  Containers::Vector<std::pair<U32, RenderPassType>> m_renderSequence;

  Containers::Vector<D3D12ScopedImage> m_targetImages;
  Containers::Vector<D3D12ScopedBuffer> m_targetBuffers;

  Containers::Vector<D3D12ScopedRenderPass> m_renderPasses;
  Containers::Vector<D3D12ScopedComputePass> m_computePasses;

  Containers::Vector<D3D12DrawablePool> m_drawablePools;
  Containers::Vector<D3D12ComputePool> m_computePools;
  Containers::Vector<D3D12ScopedShader> m_shaders;
};

} // namespace D3D12
} // namespace Azura
