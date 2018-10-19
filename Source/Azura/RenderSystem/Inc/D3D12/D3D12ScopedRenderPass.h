#pragma once
#include "Types.h"

#include "Log/Log.h"
#include "D3D12/D3D12Core.h"
#include "D3D12/D3D12ScopedImage.h"
#include "D3D12/D3D12ScopedShader.h"
#include "D3D12/D3D12ScopedCommandBuffer.h"
#include "D3D12/D3D12ScopedSwapChain.h"

namespace Azura {
namespace Memory {
class Allocator;
}
}

namespace Azura {
namespace D3D12 {

class D3D12ScopedRenderPass {
public:
  D3D12ScopedRenderPass(U32 idx, Memory::Allocator& mainAllocator, Log logger);

  void Create(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
              const PipelinePassCreateInfo& createInfo,
              const Containers::Vector<RenderTargetCreateInfo>& pipelineBuffers,
              const Containers::Vector<D3D12ScopedImage>& pipelineBufferImages,
              const Containers::Vector<DescriptorSlot>& descriptorSlots,
              const Containers::Vector<DescriptorTableEntry>& descriptorSetTable,
              const Containers::Vector<D3D12ScopedShader>& allShaders,
              const D3D12ScopedSwapChain& swapChain,
              UINT rtvDescriptorSize,
              UINT dsvDescriptorSize);

  void CreateForSwapChain(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
                          const PipelinePassCreateInfo& createInfo,
                          const Containers::Vector<RenderTargetCreateInfo>& pipelineBuffers,
                          const Containers::Vector<D3D12ScopedImage>& pipelineBufferImages,
                          const Containers::Vector<DescriptorSlot>& descriptorSlots,
                          const Containers::Vector<DescriptorTableEntry>& descriptorSetTable,
                          const Containers::Vector<D3D12ScopedShader>& allShaders,
                          const D3D12ScopedSwapChain& swapChain,
                          UINT rtvDescriptorSize,
                          UINT dsvDescriptorSize);

  U32 GetId() const;

  ID3D12GraphicsCommandList* GetPrimaryGraphicsCommandList(U32 idx) const;

  const DescriptorCount& GetDescriptorCount() const;

  ID3D12RootSignature* GetRootSignature() const;

  void RecordImageAcquireBarrier(ID3D12GraphicsCommandList* commandList, U32 idx) const;

  void RecordPresentBarrier(ID3D12GraphicsCommandList* commandList, U32 idx) const;

  void SetRenderTargets(ID3D12GraphicsCommandList* commandList, U32 cBufferIdx, UINT rtvDescriptorSize) const;

  const Containers::Vector<std::reference_wrapper<const D3D12ScopedShader>>& GetShaders() const;
  const Containers::Vector<std::reference_wrapper<const RenderTargetCreateInfo>>& GetInputInfo() const;

  const Containers::Vector<DescriptorTableEntry>& GetRootSignatureTable() const;

  void RecordResourceBarriers(ID3D12GraphicsCommandList* commandList) const;

  void WaitForGPU(ID3D12CommandQueue* commandQueue);

  U32 GetCommandBufferCount() const;

private:
  void CreateBase(
    const Microsoft::WRL::ComPtr<ID3D12Device>& device,
    const PipelinePassCreateInfo& createInfo,
    const Containers::Vector<DescriptorSlot>& descriptorSlots,
    const Containers::Vector<DescriptorTableEntry>& descriptorSetTable,
    const Containers::Vector<RenderTargetCreateInfo>& pipelineBuffers,
    const Containers::Vector<D3D12ScopedImage>& pipelineBufferImages,
    const Containers::Vector<D3D12ScopedShader>& allShaders);

  const Log log_D3D12RenderSystem;
  U32 m_id;

  bool hasDepth{false};
  bool isTargetSwapChain{false};

  Containers::Vector<DescriptorTableEntry> m_rootSignatureTable;

  Containers::Vector<std::reference_wrapper<const D3D12ScopedShader>> m_passShaders;
  Containers::Vector<std::reference_wrapper<const RenderTargetCreateInfo>> m_passInputs;

  SmallVector<D3D12ScopedCommandBuffer, DEFAULT_FRAMES_IN_FLIGHT> m_commandBuffers;
  SmallVector<Microsoft::WRL::ComPtr<ID3D12Resource>, MAX_RENDER_PASS_OUTPUTS> m_renderOutputs;
  SmallVector<std::reference_wrapper<D3D12ScopedImage>, MAX_RENDER_PASS_OUTPUTS> m_renderInputs;
  SmallVector<Microsoft::WRL::ComPtr<ID3D12Resource>, MAX_RENDER_PASS_OUTPUTS> m_depthOutputs;
  SmallVector<std::reference_wrapper<D3D12ScopedImage>, MAX_RENDER_PASS_OUTPUTS> m_depthInputs;

  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvHeap;

  Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

  DescriptorCount m_descriptorCount{};

  ClearData m_clearData;

  U32 m_fenceValue{1};
  HANDLE m_fenceEvent{};
  Microsoft::WRL::ComPtr<ID3D12Fence> m_signalFence;
  SmallVector<Microsoft::WRL::ComPtr<ID3D12Fence>, MAX_RENDER_PASS_INPUTS> m_waitFences;
};

} // namespace D3D12
} // namespace Azura