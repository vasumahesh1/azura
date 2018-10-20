#include "D3D12/D3D12ScopedImage.h"
#include "D3D12/D3D12TypeMapping.h"
#include "D3D12/D3D12Macros.h"
#include "D3D12/D3D12ScopedBuffer.h"

#include "D3D12/d3dx12.h"


namespace Azura {
namespace D3D12 {

namespace {
DXGI_FORMAT ConvertDepthFormatForSRV(DXGI_FORMAT format) {
  if (format == DXGI_FORMAT_D32_FLOAT) {
    return DXGI_FORMAT_R32_FLOAT;
  }

  if (format == DXGI_FORMAT_D24_UNORM_S8_UINT) {
    return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
  }

  if (format == DXGI_FORMAT_D32_FLOAT_S8X24_UINT) {
    return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
  }

  if (format == DXGI_FORMAT_D16_UNORM) {
    return DXGI_FORMAT_R16_UNORM;
  }

  return DXGI_FORMAT_UNKNOWN;
}

} // namespace

void D3D12ScopedImage::Create(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
                              D3D12_RESOURCE_STATES initState,
                              D3D12_RESOURCE_FLAGS resourceFlags,
                              const TextureDesc& desc,
                              const Log& log_D3D12RenderSystem) {

  const auto format = ToDXGI_FORMAT(desc.m_format);
  VERIFY_OPT(log_D3D12RenderSystem, format, "Unknown Format");

  m_format = desc.m_format;

  m_currentState = initState;

  const auto resourceDimension = ToD3D12_RESOURCE_DIMENSION(desc.m_type);
  VERIFY_OPT(log_D3D12RenderSystem, resourceDimension, "Unknown Resource Dimension");

  D3D12_RESOURCE_DESC textureDesc = {};
  textureDesc.MipLevels           = UINT16(desc.m_mipLevels);
  textureDesc.Format              = format.value();
  textureDesc.Width               = desc.m_bounds.m_width;
  textureDesc.Height              = desc.m_bounds.m_height;
  textureDesc.Flags               = resourceFlags;
  textureDesc.DepthOrArraySize    = UINT16(desc.m_bounds.m_depth > 1 ? desc.m_bounds.m_depth : desc.m_arrayLayers);
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

void D3D12ScopedImage::Transition(ID3D12GraphicsCommandList* commandList,
  D3D12_RESOURCE_STATES toState) const {
  if (m_currentState == toState)
  {
    return;
  }

  const auto resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_texture.Get(), m_currentState, toState);
  commandList->ResourceBarrier(1, &resourceBarrier);
}

D3D12_SHADER_RESOURCE_VIEW_DESC D3D12ScopedImage::GetSRV(RawStorageFormat viewFormat,
                                                         ImageViewType imageView,
                                                         const Log& log_D3D12RenderSystem) {
  const auto format = ToDXGI_FORMAT(viewFormat);
  VERIFY_OPT(log_D3D12RenderSystem, format, "Unknown Format");

  const auto srvView = ToD3D12_SRV_DIMENSION(imageView);
  VERIFY_OPT(log_D3D12RenderSystem, format, "Unknown SRV View Dimensions");

  DXGI_FORMAT srvFormat = format.value();
  if (HasDepthComponent(viewFormat) || HasStencilComponent(viewFormat)) {
    srvFormat = ConvertDepthFormatForSRV(srvFormat);
  }

  D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
  srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
  srvDesc.Format                          = srvFormat;
  srvDesc.ViewDimension                   = srvView.value();
  srvDesc.Texture2D.MipLevels             = 1; // NOLINT

  return srvDesc;
}

D3D12_DEPTH_STENCIL_VIEW_DESC D3D12ScopedImage::GetDSV(RawStorageFormat viewFormat,
                                                       ImageViewType imageView,
                                                       const Log& log_D3D12RenderSystem) {

  if (!HasDepthComponent(viewFormat) && !HasStencilComponent(viewFormat)) {
    LOG_ERR(log_D3D12RenderSystem, LOG_LEVEL, "Unknown Format Requested for DSV: %s", ToString(viewFormat).c_str());
    return {};
  }

  const auto format = ToDXGI_FORMAT(viewFormat);
  VERIFY_OPT(log_D3D12RenderSystem, format, "Unknown Format");

  const auto dsvDimension = ToD3D12_DSV_DIMENSION(imageView);
  VERIFY_OPT(log_D3D12RenderSystem, format, "Unknown SRV View Dimensions");

  D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
  dsvDesc.Texture2D.MipSlice            = 0; // NOLINT
  dsvDesc.Format                        = format.value();
  dsvDesc.ViewDimension                 = dsvDimension.value();
  dsvDesc.Flags                         = D3D12_DSV_FLAG_NONE;

  return dsvDesc;
}

D3D12_RENDER_TARGET_VIEW_DESC D3D12ScopedImage::GetRTV(RawStorageFormat viewFormat,
                                                       ImageViewType imageView,
                                                       const Log& log_D3D12RenderSystem) {

  if (HasDepthComponent(viewFormat) || HasStencilComponent(viewFormat)) {
    LOG_ERR(log_D3D12RenderSystem, LOG_LEVEL, "Unknown Format Requested for RTV: %s", ToString(viewFormat).c_str());
    return {};
  }

  const auto format = ToDXGI_FORMAT(viewFormat);
  VERIFY_OPT(log_D3D12RenderSystem, format, "Unknown Format");

  const auto rtvDimension = ToD3D12_RTV_DIMENSION(imageView);
  VERIFY_OPT(log_D3D12RenderSystem, format, "Unknown SRV View Dimensions");

  D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
  rtvDesc.Texture2D.MipSlice            = 0; // NOLINT
  rtvDesc.Texture2D.PlaneSlice          = 0; // NOLINT
  rtvDesc.Format                        = format.value();
  rtvDesc.ViewDimension                 = rtvDimension.value();

  return rtvDesc;
}

ID3D12Resource* D3D12ScopedImage::Real() const {
  return m_texture.Get();
}

RawStorageFormat D3D12ScopedImage::GetFormat() const {
  return m_format;
}

Microsoft::WRL::ComPtr<ID3D12Resource> D3D12ScopedImage::RealComPtr() const {
  return m_texture;
}

void D3D12ScopedImage::CopyFromBuffer(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
                                      ID3D12GraphicsCommandList* commandList,
                                      const D3D12ScopedBuffer& sourceBuffer,
                                      UINT64 sourceOffset) const {
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
