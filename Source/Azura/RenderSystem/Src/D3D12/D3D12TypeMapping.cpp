#include "D3D12/D3D12TypeMapping.h"


namespace Azura {
namespace D3D12 {

#define RAW_STORAGE_TO_DXGI_FORMAT_MAPPING(FUNC)                                      \
  FUNC(RawStorageFormat::UNKNOWN, DXGI_FORMAT_UNKNOWN)                                \
  FUNC(RawStorageFormat::R32_UNORM, DXGI_FORMAT_UNKNOWN)                              \
  FUNC(RawStorageFormat::R32_SNORM, DXGI_FORMAT_UNKNOWN)                              \
  FUNC(RawStorageFormat::B8G8R8A8_UNORM, DXGI_FORMAT_B8G8R8A8_UNORM)                  \
  FUNC(RawStorageFormat::B8G8R8A8_SNORM, DXGI_FORMAT_UNKNOWN)                         \
  FUNC(RawStorageFormat::B8G8R8A8_USCALED, DXGI_FORMAT_UNKNOWN)                       \
  FUNC(RawStorageFormat::B8G8R8A8_SSCALED, DXGI_FORMAT_UNKNOWN)                       \
  FUNC(RawStorageFormat::B8G8R8A8_UINT, DXGI_FORMAT_UNKNOWN)                          \
  FUNC(RawStorageFormat::B8G8R8A8_SINT, DXGI_FORMAT_UNKNOWN)                          \
  FUNC(RawStorageFormat::B8G8R8A8_SRGB, DXGI_FORMAT_UNKNOWN)                          \
  FUNC(RawStorageFormat::R8G8B8_UNORM, DXGI_FORMAT_UNKNOWN)                           \
  FUNC(RawStorageFormat::R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM)                  \
  FUNC(RawStorageFormat::R16_FLOAT, DXGI_FORMAT_R16_FLOAT)                            \
  FUNC(RawStorageFormat::R16_UINT, DXGI_FORMAT_R16_UINT)                              \
  FUNC(RawStorageFormat::R16_SINT, DXGI_FORMAT_R16_SINT)                              \
  FUNC(RawStorageFormat::R16_UNORM, DXGI_FORMAT_R16_UNORM)                            \
  FUNC(RawStorageFormat::R16_SNORM, DXGI_FORMAT_R16_SNORM)                            \
  FUNC(RawStorageFormat::R16_USCALED, DXGI_FORMAT_UNKNOWN)                            \
  FUNC(RawStorageFormat::R16_SSCALED, DXGI_FORMAT_UNKNOWN)                            \
  FUNC(RawStorageFormat::R32_FLOAT, DXGI_FORMAT_R32_FLOAT)                            \
  FUNC(RawStorageFormat::R32_UINT, DXGI_FORMAT_R32_UINT)                              \
  FUNC(RawStorageFormat::R32_SINT, DXGI_FORMAT_R32_SINT)                              \
  FUNC(RawStorageFormat::R16G16_FLOAT, DXGI_FORMAT_R16G16_FLOAT)                      \
  FUNC(RawStorageFormat::R16G16_UINT, DXGI_FORMAT_R16G16_UINT)                        \
  FUNC(RawStorageFormat::R16G16_SINT, DXGI_FORMAT_R16G16_SINT)                        \
  FUNC(RawStorageFormat::R16G16_UNORM, DXGI_FORMAT_R16G16_UNORM)                      \
  FUNC(RawStorageFormat::R16G16_SNORM, DXGI_FORMAT_R16G16_SNORM)                      \
  FUNC(RawStorageFormat::R16G16_USCALED, DXGI_FORMAT_UNKNOWN)                         \
  FUNC(RawStorageFormat::R16G16_SSCALED, DXGI_FORMAT_UNKNOWN)                         \
  FUNC(RawStorageFormat::R11G11B10_FLOAT, DXGI_FORMAT_UNKNOWN)                        \
  FUNC(RawStorageFormat::R16G16B16_FLOAT, DXGI_FORMAT_UNKNOWN)                        \
  FUNC(RawStorageFormat::R16G16B16_UINT, DXGI_FORMAT_UNKNOWN)                         \
  FUNC(RawStorageFormat::R16G16B16_SINT, DXGI_FORMAT_UNKNOWN)                         \
  FUNC(RawStorageFormat::R16G16B16_UNORM, DXGI_FORMAT_UNKNOWN)                        \
  FUNC(RawStorageFormat::R16G16B16_SNORM, DXGI_FORMAT_UNKNOWN)                        \
  FUNC(RawStorageFormat::R16G16B16_USCALED, DXGI_FORMAT_UNKNOWN)                      \
  FUNC(RawStorageFormat::R16G16B16_SSCALED, DXGI_FORMAT_UNKNOWN)                      \
  FUNC(RawStorageFormat::R64_FLOAT, DXGI_FORMAT_UNKNOWN)                              \
  FUNC(RawStorageFormat::R64_UINT, DXGI_FORMAT_UNKNOWN)                               \
  FUNC(RawStorageFormat::R64_SINT, DXGI_FORMAT_UNKNOWN)                               \
  FUNC(RawStorageFormat::R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT)                      \
  FUNC(RawStorageFormat::R32G32_UINT, DXGI_FORMAT_R32G32_UINT)                        \
  FUNC(RawStorageFormat::R32G32_SINT, DXGI_FORMAT_R32G32_SINT)                        \
  FUNC(RawStorageFormat::R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT)          \
  FUNC(RawStorageFormat::R16G16B16A16_UINT, DXGI_FORMAT_R16G16B16A16_UINT)            \
  FUNC(RawStorageFormat::R16G16B16A16_SINT, DXGI_FORMAT_R16G16B16A16_SINT)            \
  FUNC(RawStorageFormat::R16G16B16A16_UNORM, DXGI_FORMAT_R16G16B16A16_UNORM)          \
  FUNC(RawStorageFormat::R16G16B16A16_SNORM, DXGI_FORMAT_R16G16B16A16_SNORM)          \
  FUNC(RawStorageFormat::R16G16B16A16_USCALED, DXGI_FORMAT_UNKNOWN)                   \
  FUNC(RawStorageFormat::R16G16B16A16_SSCALED, DXGI_FORMAT_UNKNOWN)                   \
  FUNC(RawStorageFormat::R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT)                \
  FUNC(RawStorageFormat::R32G32B32_UINT, DXGI_FORMAT_R32G32B32_UINT)                  \
  FUNC(RawStorageFormat::R32G32B32_SINT, DXGI_FORMAT_R32G32B32_SINT)                  \
  FUNC(RawStorageFormat::R64G64_FLOAT, DXGI_FORMAT_UNKNOWN)                           \
  FUNC(RawStorageFormat::R64G64_UINT, DXGI_FORMAT_UNKNOWN)                            \
  FUNC(RawStorageFormat::R64G64_SINT, DXGI_FORMAT_UNKNOWN)                            \
  FUNC(RawStorageFormat::R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT)          \
  FUNC(RawStorageFormat::R32G32B32A32_UINT, DXGI_FORMAT_R32G32B32A32_UINT)            \
  FUNC(RawStorageFormat::R32G32B32A32_SINT, DXGI_FORMAT_R32G32B32A32_SINT)            \
  FUNC(RawStorageFormat::R64G64B64_FLOAT, DXGI_FORMAT_UNKNOWN)                        \
  FUNC(RawStorageFormat::R64G64B64_UINT, DXGI_FORMAT_UNKNOWN)                         \
  FUNC(RawStorageFormat::R64G64B64_SINT, DXGI_FORMAT_UNKNOWN)                         \
  FUNC(RawStorageFormat::R64G64B64A64_FLOAT, DXGI_FORMAT_UNKNOWN)                     \
  FUNC(RawStorageFormat::R64G64B64A64_UINT, DXGI_FORMAT_UNKNOWN)                      \
  FUNC(RawStorageFormat::R64G64B64A64_SINT, DXGI_FORMAT_UNKNOWN)                      \
  FUNC(RawStorageFormat::D24_UNORM_S8_UINT, DXGI_FORMAT_D24_UNORM_S8_UINT)            \
  FUNC(RawStorageFormat::D32_FLOAT_S8X24_UINT, DXGI_FORMAT_UNKNOWN)                   \
  FUNC(RawStorageFormat::D32_FLOAT, DXGI_FORMAT_D32_FLOAT)


#define BUFFER_USAGE_RATE_TO_INPUT_CLASSIFICATION(FUNC)                                                       \
  FUNC(BufferUsageRate::PerVertex, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA)                                \
  FUNC(BufferUsageRate::PerInstance, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA)

#define IMAGE_TYPE_TO_RESOURCE_DIMENSION(FUNC)                    \
  FUNC(ImageType::Image1D, D3D12_RESOURCE_DIMENSION_TEXTURE1D)    \
  FUNC(ImageType::Image2D, D3D12_RESOURCE_DIMENSION_TEXTURE2D)    \
  FUNC(ImageType::Image3D, D3D12_RESOURCE_DIMENSION_TEXTURE3D)

#define IMAGE_VIEW_TYPE_TO_SRV_DIMENSION(FUNC)                                            \
  FUNC(ImageViewType::ImageView1D, D3D12_SRV_DIMENSION_TEXTURE1D)                         \
  FUNC(ImageViewType::ImageView1DArray, D3D12_SRV_DIMENSION_TEXTURE1DARRAY)               \
  FUNC(ImageViewType::ImageView2D, D3D12_SRV_DIMENSION_TEXTURE2D)                         \
  FUNC(ImageViewType::ImageView2DArray, D3D12_SRV_DIMENSION_TEXTURE2DARRAY)               \
  FUNC(ImageViewType::ImageView3D, D3D12_SRV_DIMENSION_TEXTURE3D)                         \
  FUNC(ImageViewType::ImageViewCubeMap, D3D12_SRV_DIMENSION_TEXTURECUBE)                  \
  FUNC(ImageViewType::ImageViewCubeMapArray, D3D12_SRV_DIMENSION_TEXTURECUBEARRAY)

#define IMAGE_VIEW_TYPE_TO_UAV_DIMENSION(FUNC)                                            \
  FUNC(ImageViewType::ImageView1D, D3D12_UAV_DIMENSION_TEXTURE1D)                         \
  FUNC(ImageViewType::ImageView1DArray, D3D12_UAV_DIMENSION_TEXTURE1DARRAY)               \
  FUNC(ImageViewType::ImageView2D, D3D12_UAV_DIMENSION_TEXTURE2D)                         \
  FUNC(ImageViewType::ImageView2DArray, D3D12_UAV_DIMENSION_TEXTURE2DARRAY)               \
  FUNC(ImageViewType::ImageView3D, D3D12_UAV_DIMENSION_TEXTURE3D)                         \
  FUNC(ImageViewType::ImageViewCubeMap, D3D12_UAV_DIMENSION_UNKNOWN)                      \
  FUNC(ImageViewType::ImageViewCubeMapArray, D3D12_UAV_DIMENSION_UNKNOWN)

#define IMAGE_VIEW_TYPE_TO_DSV_DIMENSION(FUNC)                                            \
  FUNC(ImageViewType::ImageView1D, D3D12_DSV_DIMENSION_TEXTURE1D)                         \
  FUNC(ImageViewType::ImageView1DArray, D3D12_DSV_DIMENSION_TEXTURE1DARRAY)               \
  FUNC(ImageViewType::ImageView2D, D3D12_DSV_DIMENSION_TEXTURE2D)                         \
  FUNC(ImageViewType::ImageView2DArray, D3D12_DSV_DIMENSION_TEXTURE2DARRAY)               \
  FUNC(ImageViewType::ImageView3D, D3D12_DSV_DIMENSION_UNKNOWN)                         \
  FUNC(ImageViewType::ImageViewCubeMap, D3D12_DSV_DIMENSION_UNKNOWN)                  \
  FUNC(ImageViewType::ImageViewCubeMapArray, D3D12_DSV_DIMENSION_UNKNOWN)


#define IMAGE_VIEW_TYPE_TO_RTV_DIMENSION(FUNC)                                            \
  FUNC(ImageViewType::ImageView1D, D3D12_RTV_DIMENSION_TEXTURE1D)                         \
  FUNC(ImageViewType::ImageView1DArray, D3D12_RTV_DIMENSION_TEXTURE1DARRAY)               \
  FUNC(ImageViewType::ImageView2D, D3D12_RTV_DIMENSION_TEXTURE2D)                         \
  FUNC(ImageViewType::ImageView2DArray, D3D12_RTV_DIMENSION_TEXTURE2DARRAY)               \
  FUNC(ImageViewType::ImageView3D, D3D12_RTV_DIMENSION_TEXTURE3D)                         \
  FUNC(ImageViewType::ImageViewCubeMap, D3D12_RTV_DIMENSION_UNKNOWN)                  \
  FUNC(ImageViewType::ImageViewCubeMapArray, D3D12_RTV_DIMENSION_UNKNOWN)

#define CULL_MODE_TO_D3D12_CULL_MODE_MAPPING(FUNC)                                                                        \
  FUNC(CullMode::None, D3D12_CULL_MODE_NONE)                                                                              \
  FUNC(CullMode::FrontBit, D3D12_CULL_MODE_FRONT)                                                                     \
  FUNC(CullMode::BackBit, D3D12_CULL_MODE_BACK)                                                                       \
  FUNC(CullMode::FrontAndBack, D3D12_CULL_MODE_NONE)

#define TEXTURE_ADDRESS_MODE_TO_D3D12_TEXTURE_ADDRESS_MODE(FUNC)                               \
  FUNC(TextureAddressMode::Wrap, D3D12_TEXTURE_ADDRESS_MODE_WRAP)                              \
  FUNC(TextureAddressMode::Mirror, D3D12_TEXTURE_ADDRESS_MODE_MIRROR)                          \
  FUNC(TextureAddressMode::Clamp, D3D12_TEXTURE_ADDRESS_MODE_CLAMP)                            \
  FUNC(TextureAddressMode::Border, D3D12_TEXTURE_ADDRESS_MODE_BORDER)                          \
  FUNC(TextureAddressMode::MirrorOnce, D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE)

#define TEXTURE_FILTER_TO_D3D12_FILTER(FUNC)                                                            \
  FUNC(TextureFilter::MinMagMipPoint, D3D12_FILTER_MIN_MAG_MIP_POINT)                                   \
  FUNC(TextureFilter::MinMagPoint_MipLinear, D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR)                     \
  FUNC(TextureFilter::MinPoint_MagLinear_MipPoint, D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT)         \
  FUNC(TextureFilter::MinPoint_MagMipLinear, D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR)                     \
  FUNC(TextureFilter::MinLinear_MagMipPoint, D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT)                     \
  FUNC(TextureFilter::MinLinear_MagPoint_MipLinear, D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR)       \
  FUNC(TextureFilter::MinMagLinear_MipPoint, D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT)                     \
  FUNC(TextureFilter::MinMagMipLinear, D3D12_FILTER_MIN_MAG_MIP_LINEAR)                                 \
  FUNC(TextureFilter::Anisotropic, D3D12_FILTER_ANISOTROPIC)


#define BLEND_OP_TO_D3D12_BLEND_OP(FUNC)                       \
  FUNC(BlendOp::Add, D3D12_BLEND_OP_ADD)                       \
  FUNC(BlendOp::Subtract, D3D12_BLEND_OP_SUBTRACT)             \
  FUNC(BlendOp::ReverseSubtract, D3D12_BLEND_OP_REV_SUBTRACT)  \
  FUNC(BlendOp::Min, D3D12_BLEND_OP_MIN)                       \
  FUNC(BlendOp::Max, D3D12_BLEND_OP_MAX)

#define BLEND_OP_TO_D3D12_BLEND(FUNC)                                  \
  FUNC(BlendFactor::Zero, D3D12_BLEND_ZERO)                            \
  FUNC(BlendFactor::One, D3D12_BLEND_ONE)                              \
  FUNC(BlendFactor::SrcColor, D3D12_BLEND_SRC_COLOR)                   \
  FUNC(BlendFactor::OneMinusSrcColor, D3D12_BLEND_INV_SRC_COLOR)       \
  FUNC(BlendFactor::DstColor, D3D12_BLEND_DEST_COLOR)                  \
  FUNC(BlendFactor::OneMinusDstColor, D3D12_BLEND_INV_DEST_COLOR)      \
  FUNC(BlendFactor::SrcAlpha, D3D12_BLEND_SRC_ALPHA)                   \
  FUNC(BlendFactor::OneMinusSrcAlpha, D3D12_BLEND_INV_SRC_ALPHA)       \
  FUNC(BlendFactor::DstAlpha, D3D12_BLEND_DEST_ALPHA)                  \
  FUNC(BlendFactor::OneMinusDstAlpha, D3D12_BLEND_INV_DEST_ALPHA)      \
  FUNC(BlendFactor::ConstantColor, D3D12_BLEND_ZERO)                   \
  FUNC(BlendFactor::OneMinusConstantColor, D3D12_BLEND_ZERO)           \
  FUNC(BlendFactor::ConstantAlpha, D3D12_BLEND_ZERO)                   \
  FUNC(BlendFactor::OneMinusConstantAlpha, D3D12_BLEND_ZERO)           \
  FUNC(BlendFactor::SrcAlphaSaturate, D3D12_BLEND_ZERO)                \
  FUNC(BlendFactor::Src1Color, D3D12_BLEND_SRC1_COLOR)                 \
  FUNC(BlendFactor::OneMinusSrc1Color, D3D12_BLEND_INV_SRC1_COLOR)     \
  FUNC(BlendFactor::Src1Alpha, D3D12_BLEND_SRC1_ALPHA)                 \
  FUNC(BlendFactor::OneMinusSrc1Alpha, D3D12_BLEND_INV_SRC1_ALPHA)



#define CREATE_MAPPER_CPP(FROM_FORMAT, TO_FORMAT, MAPPING_TABLE, CASE_MAPPING_FUNC)                                    \
  CREATE_MAPPER_H(FROM_FORMAT, TO_FORMAT) {                                                                            \
    switch (inputFormat) { MAPPING_TABLE(CASE_MAPPING_FUNC) }                                                          \
    return std::nullopt;                                                                                               \
  }

CREATE_MAPPER_CPP(RawStorageFormat, DXGI_FORMAT, RAW_STORAGE_TO_DXGI_FORMAT_MAPPING, FORWARD_MAPPING)

CREATE_MAPPER_CPP(BufferUsageRate, D3D12_INPUT_CLASSIFICATION, BUFFER_USAGE_RATE_TO_INPUT_CLASSIFICATION, FORWARD_MAPPING)

CREATE_MAPPER_CPP(ImageType, D3D12_RESOURCE_DIMENSION, IMAGE_TYPE_TO_RESOURCE_DIMENSION, FORWARD_MAPPING)
CREATE_MAPPER_CPP(ImageViewType, D3D12_SRV_DIMENSION, IMAGE_VIEW_TYPE_TO_SRV_DIMENSION, FORWARD_MAPPING)
CREATE_MAPPER_CPP(ImageViewType, D3D12_UAV_DIMENSION, IMAGE_VIEW_TYPE_TO_UAV_DIMENSION, FORWARD_MAPPING)
CREATE_MAPPER_CPP(ImageViewType, D3D12_DSV_DIMENSION, IMAGE_VIEW_TYPE_TO_DSV_DIMENSION, FORWARD_MAPPING)
CREATE_MAPPER_CPP(ImageViewType, D3D12_RTV_DIMENSION, IMAGE_VIEW_TYPE_TO_RTV_DIMENSION, FORWARD_MAPPING)
CREATE_MAPPER_CPP(CullMode, D3D12_CULL_MODE, CULL_MODE_TO_D3D12_CULL_MODE_MAPPING, FORWARD_MAPPING)
CREATE_MAPPER_CPP(TextureAddressMode, D3D12_TEXTURE_ADDRESS_MODE, TEXTURE_ADDRESS_MODE_TO_D3D12_TEXTURE_ADDRESS_MODE, FORWARD_MAPPING)
CREATE_MAPPER_CPP(TextureFilter, D3D12_FILTER, TEXTURE_FILTER_TO_D3D12_FILTER, FORWARD_MAPPING)
CREATE_MAPPER_CPP(BlendOp, D3D12_BLEND_OP, BLEND_OP_TO_D3D12_BLEND_OP, FORWARD_MAPPING)
CREATE_MAPPER_CPP(BlendFactor, D3D12_BLEND, BLEND_OP_TO_D3D12_BLEND, FORWARD_MAPPING)

} // namespace D3D12
} // namespace Azura