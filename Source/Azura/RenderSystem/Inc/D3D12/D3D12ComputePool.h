#pragma once
#include "Generic/ComputePool.h"
#include "Log/Log.h"

#include "D3D12/D3D12Core.h"
#include "D3D12/D3D12ScopedPipeline.h"
#include "D3D12/D3D12Drawable.h"
#include "D3D12/D3D12ScopedBuffer.h"
#include "D3D12/D3D12ScopedCommandBuffer.h"
#include "D3D12/D3D12ScopedImage.h"
#include "D3D12/D3D12ScopedSampler.h"
#include "D3D12/D3D12ScopedComputePass.h"


namespace Azura {
namespace D3D12 {

struct D3D12ComputePassRecordEntry
{
  ID3D12PipelineState* m_pso;
  ID3D12GraphicsCommandList* m_bundle;
  U32 m_poolIdx;
};

class D3D12ComputePool : public ComputePool {
  
public:
  D3D12ComputePool(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
                    const ComputePoolCreateInfo& createInfo,
                    const DescriptorCount& descriptorCount,
                    const Containers::Vector<DescriptorSlot>& descriptorSlots,
                    const Containers::Vector<D3D12ScopedShader>& shaders,
                    const Containers::Vector<D3D12ScopedComputePass>& renderPasses,
                    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue,
                    Microsoft::WRL::ComPtr<ID3D12CommandQueue> graphicsQueue,
                    Memory::Allocator& mainAllocator,
                    Memory::Allocator& initAllocator,
                    Log log);

  void AddShader(U32 shaderId) override;
  void BindTextureData(SlotID slot, const TextureDesc& desc, const U8* buffer) override;
  void BindSampler(SlotID slot, const SamplerDesc& desc) override;
  void SetUniformBufferData();
  void BindUniformData(SlotID slot, const U8* buffer, U32 size) override;
  void Submit() override;
  void Record();

  void BeginUpdates() override;
  void UpdateUniformData(SlotID slot, const U8* buffer, U32 size) override;
  void UpdateTextureData(SlotID slot, const U8* buffer) override;
  void SubmitUpdates() override;

  const Containers::Vector<ID3D12DescriptorHeap*>& GetAllDescriptorHeaps() const;
  ID3D12PipelineState* GetPipelineState(U32 renderPassId) const;

private:
  void CreateRenderPassReferences(const ComputePoolCreateInfo& createInfo, const Containers::Vector<D3D12ScopedComputePass>& renderPasses);
  void CreateDescriptorHeap();

  void SetTextureData(ID3D12GraphicsCommandList* oneTimeCommandList);
  void SetSamplerData();

  void CreateComputePassInputTargetSRV(const Containers::Vector<std::reference_wrapper<D3D12ScopedImage>>& renderPassInputs, U32 offsetTillThis) const;
  void CreateComputePassInputBufferSRV(
    const Containers::Vector<std::reference_wrapper<D3D12ScopedBuffer>>& bufferInputs,
    U32 offsetTillThis) const;
  void CreateComputePassOutputTargetUAV(
    const Containers::Vector<std::reference_wrapper<D3D12ScopedImage>>& computePassOutputs,
    U32 offsetTillThis) const;

  void CreateComputePassOutputBufferUAV(const Containers::Vector<std::reference_wrapper<D3D12ScopedBuffer>>& computePassOutputs, U32 offsetTillThis) const;

  Log log_D3D12RenderSystem;

  const Microsoft::WRL::ComPtr<ID3D12Device>& m_device;
  const Containers::Vector<DescriptorSlot>& m_globalDescriptorSlots;
  const Containers::Vector<D3D12ScopedShader>& m_shaders;

  Containers::Vector<D3D12ScopedPipeline> m_pipelines;

  Containers::Vector<std::reference_wrapper<D3D12ScopedComputePass>> m_computePasses;

  Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_computeCommandQueue;
  Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_graphicsCommandQueue;

  D3D12PipelineFactory m_pipelineFactory;
  D3D12ScopedBuffer m_updateBuffer;
  D3D12ScopedBuffer m_stagingBuffer;
  D3D12ScopedBuffer m_mainBuffer;

  U32 m_cbvSrvDescriptorElementSize{0};
  U32 m_samplerDescriptorElementSize{0};
  U32 m_offsetToConstantBuffers{0};
  U32 m_offsetToComputePassInputTargets{0};
  U32 m_offsetToComputePassInputBuffers{0};
  U32 m_offsetToComputePassOutputTargets{0};
  U32 m_offsetToComputePassOutputBuffers{0};

  Containers::Vector<D3D12ScopedImage> m_images;
  Containers::Vector<D3D12ScopedSampler> m_samplers;

  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descriptorComputeHeap;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descriptorSamplerHeap;

  Containers::Vector<ID3D12DescriptorHeap*> m_allHeaps;
};

} // namespace D3D12
} // namespace Azura
