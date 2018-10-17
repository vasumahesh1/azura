#pragma once
#include "Generic/Drawable.h"
#include "Log/Log.h"

#include "D3D12/D3D12Core.h"
#include "D3D12/D3D12ScopedPipeline.h"
#include "D3D12/D3D12Drawable.h"


namespace Azura {
namespace D3D12 {

class D3D12DrawablePool : public DrawablePool {

public:
  D3D12DrawablePool(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
                    const DrawablePoolCreateInfo& createInfo,
                    const DescriptorCount& descriptorCount,
                    const Containers::Vector<D3D12ScopedShader>& shaders,
                    Memory::Allocator& mainAllocator,
                    Memory::Allocator& initAllocator,
                    Log log);

  DrawableID CreateDrawable(const DrawableCreateInfo& createInfo) override;

  void BindVertexData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) override;
  void BindInstanceData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) override;
  void BindUniformData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) override;
  void SetIndexData(DrawableID drawableId, const U8* buffer, U32 size) override;
  void AddShader(U32 shaderId) override;
  void BindTextureData(SlotID slot, const TextureDesc& desc, const U8* buffer) override;
  void BindSampler(SlotID slot, const SamplerDesc& desc) override;
  void Submit() override;

  ID3D12DescriptorHeap* GetDescriptorHeap() const;
  ID3D12RootSignature* GetRootSignature() const;

private:
  void CreateRootSignature(const Microsoft::WRL::ComPtr<ID3D12Device>& device);
  void CreateInputAttributes(const DrawablePoolCreateInfo& createInfo);
  void CreateDescriptorHeap(const DrawablePoolCreateInfo& createInfo);

  Log log_D3D12RenderSystem;

  const Microsoft::WRL::ComPtr<ID3D12Device>& m_device;
  const Containers::Vector<D3D12ScopedShader>& m_shaders;

  Containers::Vector<D3D12ScopedPipeline> m_pipelines;
  Containers::Vector<D3D12Drawable> m_drawables;

  D3D12PipelineFactory m_pipelineFactory;

  Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
};

} // namespace D3D12
} // namespace Azura
