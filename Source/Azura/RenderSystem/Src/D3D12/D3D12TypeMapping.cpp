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

#define CREATE_MAPPER_CPP(FROM_FORMAT, TO_FORMAT, MAPPING_TABLE, CASE_MAPPING_FUNC)                                    \
  CREATE_MAPPER_H(FROM_FORMAT, TO_FORMAT) {                                                                            \
    switch (inputFormat) { MAPPING_TABLE(CASE_MAPPING_FUNC) }                                                          \
    return std::nullopt;                                                                                               \
  }

CREATE_MAPPER_CPP(RawStorageFormat, DXGI_FORMAT, RAW_STORAGE_TO_DXGI_FORMAT_MAPPING, FORWARD_MAPPING)

CREATE_MAPPER_CPP(BufferUsageRate, D3D12_INPUT_CLASSIFICATION, BUFFER_USAGE_RATE_TO_INPUT_CLASSIFICATION, FORWARD_MAPPING)

} // namespace D3D12
} // namespace Azura