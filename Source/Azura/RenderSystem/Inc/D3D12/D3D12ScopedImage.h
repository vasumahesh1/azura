#pragma once

#include "Log/Log.h"

#include "D3D12/D3D12Core.h"

namespace Azura {
namespace D3D12 {

class D3D12ScopedImage {
public:
  void Create(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
              D3D12_RESOURCE_STATES initState,
              const TextureDesc& desc,
              const Log& log_D3D12RenderSystem);

  static D3D12_SHADER_RESOURCE_VIEW_DESC GetSRV(const TextureDesc& desc,
                                                ImageViewType imageView,
                                                const Log& log_D3D12RenderSystem);

  ID3D12Resource* Real() const;

  void CopyFromBuffer(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
                      ID3D12GraphicsCommandList* commandList,
                      const D3D12ScopedBuffer& sourceBuffer,
                      UINT64 sourceOffset) const;

  void Transition(ID3D12GraphicsCommandList* commandList,
                  D3D12_RESOURCE_STATES fromState,
                  D3D12_RESOURCE_STATES toState) const;

private:
  Microsoft::WRL::ComPtr<ID3D12Resource> m_texture;
};

} // namespace D3D12
} // namespace Azura
