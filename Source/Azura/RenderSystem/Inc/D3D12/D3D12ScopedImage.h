#pragma once

#include "Log/Log.h"

#include "D3D12/D3D12Core.h"

namespace Azura {
namespace D3D12 {

class D3D12ScopedImage {
public:
  void Create(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
              D3D12_RESOURCE_STATES initState,
              D3D12_RESOURCE_FLAGS resourceFlags,
              const TextureDesc& desc,
              const Log& log_D3D12RenderSystem);

  static D3D12_SHADER_RESOURCE_VIEW_DESC GetSRV(RawStorageFormat viewFormat,
                                                ImageViewType imageView,
                                                const Log& log_D3D12RenderSystem);

  static D3D12_UNORDERED_ACCESS_VIEW_DESC GetUAV(RawStorageFormat viewFormat,
                                                 ImageViewType imageView,
                                                 const Log& log_D3D12RenderSystem);

  static D3D12_DEPTH_STENCIL_VIEW_DESC GetDSV(RawStorageFormat viewFormat,
                                              ImageViewType imageView,
                                              const Log& log_D3D12RenderSystem);

  static D3D12_RENDER_TARGET_VIEW_DESC GetRTV(RawStorageFormat viewFormat,
                                              ImageViewType imageView,
                                              const Log& log_D3D12RenderSystem);

  ID3D12Resource* Real() const;

  RawStorageFormat GetFormat() const;

  Microsoft::WRL::ComPtr<ID3D12Resource> RealComPtr() const;

  void CopyFromBuffer(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
                      ID3D12GraphicsCommandList* commandList,
                      const D3D12ScopedBuffer& sourceBuffer,
                      UINT64 sourceOffset) const;

  void Transition(ID3D12GraphicsCommandList* commandList,
                  D3D12_RESOURCE_STATES fromState,
                  D3D12_RESOURCE_STATES toState,
                  const Log& log_D3D12RenderSystem) const;

  void Transition(ID3D12GraphicsCommandList* commandList,
                  D3D12_RESOURCE_STATES toState,
                  const Log& log_D3D12RenderSystem);

private:
  Microsoft::WRL::ComPtr<ID3D12Resource> m_texture;
  RawStorageFormat m_format{RawStorageFormat::UNKNOWN};
  D3D12_RESOURCE_STATES m_currentState{};
};

} // namespace D3D12
} // namespace Azura
