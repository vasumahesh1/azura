#pragma once
#include "Generic/Drawable.h"
#include "Log/Log.h"

#include "D3D12/D3D12Core.h"
#include "D3D12/D3D12ScopedPipeline.h"
#include "D3D12/D3D12Drawable.h"
#include "D3D12/D3D12ScopedBuffer.h"
#include "D3D12/D3D12ScopedCommandBuffer.h"
#include "D3D12/D3D12ScopedImage.h"
#include "D3D12/D3D12ScopedSampler.h"
#include "D3D12/D3D12ScopedRenderPass.h"


namespace Azura {
namespace D3D12 {

struct D3D12RenderPassRecordEntry
{
  ID3D12PipelineState* m_pso;
  ID3D12GraphicsCommandList* m_bundle;
  U32 m_poolIdx;
};

class D3D12DrawablePool : public DrawablePool {

public:
  D3D12DrawablePool(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
                    const DrawablePoolCreateInfo& createInfo,
                    const DescriptorCount& descriptorCount,
                    const Containers::Vector<DescriptorSlot>& descriptorSlots,
                    const Containers::Vector<D3D12ScopedShader>& shaders,
                    const Containers::Vector<D3D12ScopedRenderPass>& renderPasses,
                    const Containers::Vector<D3D12ScopedBuffer>& gpuBuffers,
                    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue,
                    Memory::Allocator& mainAllocator,
                    Memory::Allocator& initAllocator,
                    Log log);

  DrawableID CreateDrawable(const DrawableCreateInfo& createInfo) override;

  void BindVertexData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) override;
  void BindVertexData(DrawableID drawableId, SlotID slot, U32 sourceBuffer, U32 offset, U32 size) override;
  void BindInstanceData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) override;
  void BindUniformData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) override;
  void SetIndexData(DrawableID drawableId, const U8* buffer, U32 size) override;
  void AddShader(U32 shaderId) override;
  void BindTextureData(SlotID slot, const TextureDesc& desc, const U8* buffer) override;
  void BindSampler(SlotID slot, const SamplerDesc& desc) override;
  void Submit() override;

  void BeginUpdates() override;
  void UpdateUniformData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) override;
  void UpdateTextureData(SlotID slot, const U8* buffer) override;
  void UpdateVertexData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) override;
  void UpdateInstanceData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) override;
  void SubmitUpdates() override;

  const Containers::Vector<ID3D12DescriptorHeap*>& GetAllDescriptorHeaps() const;
  ID3D12PipelineState* GetPipelineState(U32 renderPassId) const;
  ID3D12GraphicsCommandList* GetSecondaryCommandList(U32 renderPassId) const;

  void GetRecordEntries(Containers::Vector<std::pair<U32, D3D12RenderPassRecordEntry>>& recordList) const;

private:
  void CreateRenderPassReferences(const DrawablePoolCreateInfo& createInfo, const Containers::Vector<D3D12ScopedRenderPass>& renderPasses);
  void CreateInputAttributes(const DrawablePoolCreateInfo& createInfo);
  void CreateDescriptorHeap(const DrawablePoolCreateInfo& createInfo);

  void SetTextureData(ID3D12GraphicsCommandList* oneTimeCommandList);
  void SetSamplerData();

  void CreateRenderPassInputTargetSRV(const Containers::Vector<std::reference_wrapper<const D3D12ScopedImage>>& renderPassInputs, U32 offsetTillThis) const;

  Log log_D3D12RenderSystem;

  const Microsoft::WRL::ComPtr<ID3D12Device>& m_device;
  const Containers::Vector<DescriptorSlot>& m_globalDescriptorSlots;
  const Containers::Vector<D3D12ScopedShader>& m_shaders;

  Containers::Vector<D3D12ScopedPipeline> m_pipelines;
  Containers::Vector<D3D12Drawable> m_drawables;

  Containers::Vector<std::reference_wrapper<D3D12ScopedRenderPass>> m_renderPasses;
  Containers::Vector<std::reference_wrapper<const D3D12ScopedBuffer>> m_gpuBuffers;

  Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_graphicsCommandQueue;

  D3D12PipelineFactory m_pipelineFactory;
  D3D12ScopedBuffer m_updateBuffer;
  D3D12ScopedBuffer m_stagingBuffer;
  D3D12ScopedBuffer m_mainBuffer;

  U32 m_descriptorsPerDrawable;

  U32 m_cbvSrvDescriptorElementSize{0};
  U32 m_samplerDescriptorElementSize{0};
  U32 m_offsetToDrawableHeap{0};
  U32 m_offsetToRenderPassInputs{0};

  Containers::Vector<D3D12ScopedImage> m_images;
  Containers::Vector<D3D12ScopedSampler> m_samplers;
  Containers::Vector<D3D12ScopedCommandBuffer> m_secondaryCommandBuffers;

  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descriptorDrawableHeap;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descriptorSamplerHeap;

  Containers::Vector<ID3D12DescriptorHeap*> m_allHeaps;
};

} // namespace D3D12
} // namespace Azura
