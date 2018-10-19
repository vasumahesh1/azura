#include "D3D12/D3D12ScopedImage.h"
#include "D3D12/D3D12TypeMapping.h"
#include "D3D12/D3D12Macros.h"
#include "D3D12/D3D12ScopedBuffer.h"

#include "D3D12/d3dx12.h"


namespace Azura {
namespace D3D12 {

void D3D12ScopedImage::Create(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
                              D3D12_RESOURCE_STATES initState,
                              const TextureDesc& desc,
                              const Log& log_D3D12RenderSystem) {

  const auto format = ToDXGI_FORMAT(desc.m_format);
  VERIFY_OPT(log_D3D12RenderSystem, format, "Unknown Format");

  const auto resourceDimension = ToD3D12_RESOURCE_DIMENSION(desc.m_type);
  VERIFY_OPT(log_D3D12RenderSystem, resourceDimension, "Unknown Resource Dimension");

  D3D12_RESOURCE_DESC textureDesc = {};
  textureDesc.MipLevels           = UINT16(desc.m_mipLevels);
  textureDesc.Format              = format.value();
  textureDesc.Width               = desc.m_bounds.m_width;
  textureDesc.Height              = desc.m_bounds.m_height;
  textureDesc.Flags               = D3D12_RESOURCE_FLAG_NONE;
  textureDesc.DepthOrArraySize    = UINT16(desc.m_bounds.m_depth > 0 ? desc.m_bounds.m_depth : desc.m_arrayLayers);
  textureDesc.SampleDesc.Count    = 1;
  textureDesc.SampleDesc.Quality  = 0;
  textureDesc.Dimension           = resourceDimension.value();

  const auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

  VERIFY_D3D_OP(log_D3D12RenderSystem, device->CreateCommittedResource(
    &heapProperties,
    D3D12_HEAP_FLAG_NONE,
    &textureDesc,
    initState,
    nullptr,
    IID_PPV_ARGS(&m_texture)), "Failed to create image comitted resource");
}

void D3D12ScopedImage::Transition(ID3D12GraphicsCommandList* commandList,
                                  D3D12_RESOURCE_STATES fromState,
                                  D3D12_RESOURCE_STATES toState) const {
  const auto resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_texture.Get(), fromState, toState);
  commandList->ResourceBarrier(1, &resourceBarrier);
}

D3D12_SHADER_RESOURCE_VIEW_DESC D3D12ScopedImage::GetSRV(const TextureDesc& desc,
                                                         ImageViewType imageView,
                                                         const Log& log_D3D12RenderSystem) {
  const auto format = ToDXGI_FORMAT(desc.m_format);
  VERIFY_OPT(log_D3D12RenderSystem, format, "Unknown Format");

  const auto srvView = ToD3D12_SRV_DIMENSION(imageView);
  VERIFY_OPT(log_D3D12RenderSystem, format, "Unknown SRV View Dimensions");

  D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
  srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
  srvDesc.Format                          = format.value();
  srvDesc.ViewDimension                   = srvView.value();
  srvDesc.Texture2D.MipLevels             = 1;

  return srvDesc;
}

ID3D12Resource* D3D12ScopedImage::Real() const {
  return m_texture.Get();
}

void D3D12ScopedImage::CopyFromBuffer(const Microsoft::WRL::ComPtr<ID3D12Device>& device, ID3D12GraphicsCommandList* commandList, const D3D12ScopedBuffer& sourceBuffer, UINT64 sourceOffset) const {
  const auto textureDesc = m_texture->GetDesc();

  D3D12_PLACED_SUBRESOURCE_FOOTPRINT layouts;
  UINT64 rowSizesInBytes;
  UINT numRows;
  UINT64 requiredSize;

  device->GetCopyableFootprints(&textureDesc, 0, 1, sourceOffset, &layouts, &numRows, &rowSizesInBytes, &requiredSize);

  CD3DX12_TEXTURE_COPY_LOCATION Dst(m_texture.Get(), 0);
  CD3DX12_TEXTURE_COPY_LOCATION Src(sourceBuffer.Real(), layouts);
  commandList->CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);
}

} // namespace D3D12
} // namespace Azura
