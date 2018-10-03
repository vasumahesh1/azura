#include "Vulkan/VkTypeMapping.h"
#include "Utils/Macros.h"

namespace Azura {
namespace Vulkan {

#define RAW_STORAGE_TO_VK_FORMAT_MAPPING(FUNC)                                                                         \
  FUNC(RawStorageFormat::UNKNOWN, VK_FORMAT_UNDEFINED)                                                                 \
  FUNC(RawStorageFormat::R32_UNORM, VK_FORMAT_UNDEFINED)                                                               \
  FUNC(RawStorageFormat::R32_SNORM, VK_FORMAT_UNDEFINED)                                                               \
                                                                                                                       \
  FUNC(RawStorageFormat::B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_UNORM)                                                     \
  FUNC(RawStorageFormat::B8G8R8A8_SNORM, VK_FORMAT_B8G8R8A8_SNORM)                                                     \
  FUNC(RawStorageFormat::B8G8R8A8_USCALED, VK_FORMAT_B8G8R8A8_USCALED)                                                 \
  FUNC(RawStorageFormat::B8G8R8A8_SSCALED, VK_FORMAT_B8G8R8A8_SSCALED)                                                 \
  FUNC(RawStorageFormat::B8G8R8A8_UINT, VK_FORMAT_B8G8R8A8_UINT)                                                       \
  FUNC(RawStorageFormat::B8G8R8A8_SINT, VK_FORMAT_B8G8R8A8_SINT)                                                       \
  FUNC(RawStorageFormat::B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_SRGB)                                                       \
  FUNC(RawStorageFormat::R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM)                                                     \
  FUNC(RawStorageFormat::R16_FLOAT, VK_FORMAT_R16_SFLOAT)                                                              \
  FUNC(RawStorageFormat::R16_UINT, VK_FORMAT_R16_UINT)                                                                 \
  FUNC(RawStorageFormat::R16_SINT, VK_FORMAT_R16_SINT)                                                                 \
  FUNC(RawStorageFormat::R16_UNORM, VK_FORMAT_R16_UNORM)                                                               \
  FUNC(RawStorageFormat::R16_SNORM, VK_FORMAT_R16_SNORM)                                                               \
  FUNC(RawStorageFormat::R16_USCALED, VK_FORMAT_R16_USCALED)                                                           \
  FUNC(RawStorageFormat::R16_SSCALED, VK_FORMAT_R16_SSCALED)                                                           \
  FUNC(RawStorageFormat::R32_FLOAT, VK_FORMAT_R32_SFLOAT)                                                              \
  FUNC(RawStorageFormat::R32_UINT, VK_FORMAT_R32_UINT)                                                                 \
  FUNC(RawStorageFormat::R32_SINT, VK_FORMAT_R32_SINT)                                                                 \
  FUNC(RawStorageFormat::R16G16_FLOAT, VK_FORMAT_R16G16_SFLOAT)                                                        \
  FUNC(RawStorageFormat::R16G16_UINT, VK_FORMAT_R16G16_UINT)                                                           \
  FUNC(RawStorageFormat::R16G16_SINT, VK_FORMAT_R16G16_SINT)                                                           \
  FUNC(RawStorageFormat::R16G16_UNORM, VK_FORMAT_R16G16_UNORM)                                                         \
  FUNC(RawStorageFormat::R16G16_SNORM, VK_FORMAT_R16G16_SNORM)                                                         \
  FUNC(RawStorageFormat::R16G16_USCALED, VK_FORMAT_R16G16_USCALED)                                                     \
  FUNC(RawStorageFormat::R16G16_SSCALED, VK_FORMAT_R16G16_SSCALED)                                                     \
  FUNC(RawStorageFormat::R11G11B10_FLOAT, VK_FORMAT_B10G11R11_UFLOAT_PACK32)                                           \
  FUNC(RawStorageFormat::R16G16B16_FLOAT, VK_FORMAT_R16G16B16_SFLOAT)                                                  \
  FUNC(RawStorageFormat::R16G16B16_UINT, VK_FORMAT_R16G16B16_UINT)                                                     \
  FUNC(RawStorageFormat::R16G16B16_SINT, VK_FORMAT_R16G16B16_SINT)                                                     \
  FUNC(RawStorageFormat::R16G16B16_UNORM, VK_FORMAT_R16G16B16_UNORM)                                                   \
  FUNC(RawStorageFormat::R16G16B16_SNORM, VK_FORMAT_R16G16B16_SNORM)                                                   \
  FUNC(RawStorageFormat::R16G16B16_USCALED, VK_FORMAT_R16G16B16_USCALED)                                               \
  FUNC(RawStorageFormat::R16G16B16_SSCALED, VK_FORMAT_R16G16B16_SSCALED)                                               \
  FUNC(RawStorageFormat::R64_FLOAT, VK_FORMAT_R64_SFLOAT)                                                              \
  FUNC(RawStorageFormat::R64_UINT, VK_FORMAT_R64_UINT)                                                                 \
  FUNC(RawStorageFormat::R64_SINT, VK_FORMAT_R64_SINT)                                                                 \
  FUNC(RawStorageFormat::R32G32_FLOAT, VK_FORMAT_R32G32_SFLOAT)                                                        \
  FUNC(RawStorageFormat::R32G32_UINT, VK_FORMAT_R32G32_UINT)                                                           \
  FUNC(RawStorageFormat::R32G32_SINT, VK_FORMAT_R32G32_SINT)                                                           \
  FUNC(RawStorageFormat::R16G16B16A16_FLOAT, VK_FORMAT_R16G16B16A16_SFLOAT)                                            \
  FUNC(RawStorageFormat::R16G16B16A16_UINT, VK_FORMAT_R16G16B16A16_UINT)                                               \
  FUNC(RawStorageFormat::R16G16B16A16_SINT, VK_FORMAT_R16G16B16A16_SINT)                                               \
  FUNC(RawStorageFormat::R16G16B16A16_UNORM, VK_FORMAT_R16G16B16A16_UNORM)                                             \
  FUNC(RawStorageFormat::R16G16B16A16_SNORM, VK_FORMAT_R16G16B16A16_SNORM)                                             \
  FUNC(RawStorageFormat::R16G16B16A16_USCALED, VK_FORMAT_R16G16B16A16_USCALED)                                         \
  FUNC(RawStorageFormat::R16G16B16A16_SSCALED, VK_FORMAT_R16G16B16A16_SSCALED)                                         \
  FUNC(RawStorageFormat::R32G32B32_FLOAT, VK_FORMAT_R32G32B32_SFLOAT)                                                  \
  FUNC(RawStorageFormat::R32G32B32_UINT, VK_FORMAT_R32G32B32_UINT)                                                     \
  FUNC(RawStorageFormat::R32G32B32_SINT, VK_FORMAT_R32G32B32_SINT)                                                     \
  FUNC(RawStorageFormat::R64G64_FLOAT, VK_FORMAT_R64G64_SFLOAT)                                                        \
  FUNC(RawStorageFormat::R64G64_UINT, VK_FORMAT_R64G64_UINT)                                                           \
  FUNC(RawStorageFormat::R64G64_SINT, VK_FORMAT_R64G64_SINT)                                                           \
  FUNC(RawStorageFormat::R32G32B32A32_FLOAT, VK_FORMAT_R32G32B32A32_SFLOAT)                                            \
  FUNC(RawStorageFormat::R32G32B32A32_UINT, VK_FORMAT_R32G32B32A32_UINT)                                               \
  FUNC(RawStorageFormat::R32G32B32A32_SINT, VK_FORMAT_R32G32B32A32_SINT)                                               \
  FUNC(RawStorageFormat::R64G64B64_FLOAT, VK_FORMAT_R64G64B64_SFLOAT)                                                  \
  FUNC(RawStorageFormat::R64G64B64_UINT, VK_FORMAT_R64G64B64_UINT)                                                     \
  FUNC(RawStorageFormat::R64G64B64_SINT, VK_FORMAT_R64G64B64_SINT)                                                     \
  FUNC(RawStorageFormat::R64G64B64A64_FLOAT, VK_FORMAT_R64G64B64A64_SFLOAT)                                            \
  FUNC(RawStorageFormat::R64G64B64A64_UINT, VK_FORMAT_R64G64B64A64_UINT)                                               \
  FUNC(RawStorageFormat::R64G64B64A64_SINT, VK_FORMAT_R64G64B64A64_SINT)                                               \
  FUNC(RawStorageFormat::D24_UNORM_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT)                                               \
  FUNC(RawStorageFormat::D32_FLOAT_S8X24_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT)                                           \
  FUNC(RawStorageFormat::D32_FLOAT, VK_FORMAT_D32_SFLOAT)


#define RAW_STORAGE_TO_VK_INDEX_TYPE_MAPPING(FUNC)                                                                      \
  FUNC(RawStorageFormat::R16_UINT, VK_INDEX_TYPE_UINT16)                                                               \
  FUNC(RawStorageFormat::R32_UINT, VK_INDEX_TYPE_UINT32)
#define COLOR_SPACE_TO_VK_COLOR_SPACE_MAPPING(FUNC)                                                                    \
  FUNC(ColorSpace::SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)                                                            \
  FUNC(ColorSpace::HDR10, VK_COLOR_SPACE_HDR10_HLG_EXT)

#define PRESENT_MODES_TO_VK_PRESENT_MODE_MAPPING(FUNC)                                                                 \
  FUNC(PresentModes::Immediate, VK_PRESENT_MODE_IMMEDIATE_KHR)                                                         \
  FUNC(PresentModes::Mailbox, VK_PRESENT_MODE_MAILBOX_KHR)                                                             \
  FUNC(PresentModes::FIFO, VK_PRESENT_MODE_FIFO_KHR)                                                                   \
  FUNC(PresentModes::FIFORelaxed, VK_PRESENT_MODE_FIFO_RELAXED_KHR)                                                    \
  FUNC(PresentModes::SharedDemandRefresh, VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR)                                   \
  FUNC(PresentModes::SharedContinuous, VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR)

#define SHADER_STAGE_TO_SHADER_STAGE_FLAGS_MAPPING(FUNC)                                                               \
  FUNC(ShaderStage::All, VK_SHADER_STAGE_ALL)                                                                          \
  FUNC(ShaderStage::Vertex, VK_SHADER_STAGE_VERTEX_BIT)                                                                \
  FUNC(ShaderStage::Pixel, VK_SHADER_STAGE_FRAGMENT_BIT)                                                               \
  FUNC(ShaderStage::Compute, VK_SHADER_STAGE_COMPUTE_BIT)                                                              \
  FUNC(ShaderStage::Geometry, VK_SHADER_STAGE_GEOMETRY_BIT)

#define PRIMITIVE_TOPOLOGY_TO_VK_PRIMITIVE_TOPOLOGY_MAPPING(FUNC)                                                      \
  FUNC(PrimitiveTopology::PointList, VK_PRIMITIVE_TOPOLOGY_POINT_LIST)                                                 \
  FUNC(PrimitiveTopology::LineList, VK_PRIMITIVE_TOPOLOGY_LINE_LIST)                                                   \
  FUNC(PrimitiveTopology::LineStrip, VK_PRIMITIVE_TOPOLOGY_LINE_STRIP)                                                 \
  FUNC(PrimitiveTopology::TriangleList, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)                                           \
  FUNC(PrimitiveTopology::TriangleStrip, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP)                                         \
  FUNC(PrimitiveTopology::TriangleFan, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN)                                             \
  FUNC(PrimitiveTopology::LineListWithAdjacency, VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY)                       \
  FUNC(PrimitiveTopology::LineStripWithAdjacency, VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY)                     \
  FUNC(PrimitiveTopology::TriangleListWithAdjacency, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY)               \
  FUNC(PrimitiveTopology::TriangleStripWithAdjacency, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY)             \
  FUNC(PrimitiveTopology::PatchList, VK_PRIMITIVE_TOPOLOGY_PATCH_LIST)

#define CULL_MODE_TO_VK_CULL_MODE_MAPPING(FUNC)                                                                        \
  FUNC(CullMode::None, VK_CULL_MODE_NONE)                                                                              \
  FUNC(CullMode::FrontBit, VK_CULL_MODE_FRONT_BIT)                                                                     \
  FUNC(CullMode::BackBit, VK_CULL_MODE_BACK_BIT)                                                                       \
  FUNC(CullMode::FrontAndBack, VK_CULL_MODE_FRONT_AND_BACK)

#define FRONT_FACE_TO_VK_FRONT_FACE_MAPPING(FUNC)                                                                      \
  FUNC(FrontFace::CounterClockwise, VK_FRONT_FACE_COUNTER_CLOCKWISE)                                                   \
  FUNC(FrontFace::Clockwise, VK_FRONT_FACE_CLOCKWISE)

#define BUFFER_USAGE_RATE_TO_VK_VERTEX_INPUT_RATE_MAPPING(FUNC)                                                        \
  FUNC(BufferUsageRate::PerVertex, VK_VERTEX_INPUT_RATE_VERTEX)                                                        \
  FUNC(BufferUsageRate::PerInstance, VK_VERTEX_INPUT_RATE_INSTANCE)

#define IMAGE_TYPE_TO_VK_IMAGE_TYPE(FUNC)                                                        \
  FUNC(ImageType::Image1D, VK_IMAGE_TYPE_1D)                                                        \
  FUNC(ImageType::Image2D, VK_IMAGE_TYPE_2D)                                                        \
  FUNC(ImageType::Image3D, VK_IMAGE_TYPE_3D)

#define CREATE_MAPPER_CPP(FROM_FORMAT, TO_FORMAT, MAPPING_TABLE, CASE_MAPPING_FUNC)                                    \
  CREATE_MAPPER_H(FROM_FORMAT, TO_FORMAT) {                                                                            \
    switch (inputFormat) { MAPPING_TABLE(CASE_MAPPING_FUNC) }                                                          \
    return std::nullopt;                                                                                               \
  }

// TODO(vasumahesh1): Enable some mappings when fully mapped (Currently compilers raises warnings for some mappings not
// present - This is intentional)

CREATE_MAPPER_CPP(RawStorageFormat, VkFormat, RAW_STORAGE_TO_VK_FORMAT_MAPPING, FORWARD_MAPPING)
// CREATE_MAPPER_CPP(VkFormat, RawStorageFormat, RAW_STORAGE_TO_VK_FORMAT_MAPPING, REVERSE_MAPPING)

CREATE_MAPPER_CPP(RawStorageFormat, VkIndexType, RAW_STORAGE_TO_VK_INDEX_TYPE_MAPPING, FORWARD_MAPPING) // NOLINT - Exception case as VkIndexType is different from VkFormat.
// CREATE_MAPPER_CPP(RawStorageFormat, VkIndexType, RAW_STORAGE_TO_VK_INDEX_TYPE_MAPPING, REVERSE_MAPPING)

CREATE_MAPPER_CPP(ColorSpace, VkColorSpaceKHR, COLOR_SPACE_TO_VK_COLOR_SPACE_MAPPING, FORWARD_MAPPING)
// CREATE_MAPPER_CPP(VkColorSpaceKHR, ColorSpace, COLOR_SPACE_TO_VK_COLOR_SPACE_MAPPING, REVERSE_MAPPING)

CREATE_MAPPER_CPP(PresentModes, VkPresentModeKHR, PRESENT_MODES_TO_VK_PRESENT_MODE_MAPPING, FORWARD_MAPPING)
// CREATE_MAPPER_CPP(VkPresentModeKHR, PresentModes, PRESENT_MODES_TO_VK_PRESENT_MODE_MAPPING, REVERSE_MAPPING)

CREATE_MAPPER_CPP(ShaderStage, VkShaderStageFlagBits, SHADER_STAGE_TO_SHADER_STAGE_FLAGS_MAPPING, FORWARD_MAPPING)
// CREATE_MAPPER_CPP(VkShaderStageFlagBits, ShaderStage, SHADER_STAGE_TO_SHADER_STAGE_FLAGS_MAPPING, REVERSE_MAPPING)

CREATE_MAPPER_CPP(PrimitiveTopology,
  VkPrimitiveTopology,
  PRIMITIVE_TOPOLOGY_TO_VK_PRIMITIVE_TOPOLOGY_MAPPING,
  FORWARD_MAPPING)
// CREATE_MAPPER_CPP(VkPrimitiveTopology, PrimitiveTopology, PRIMITIVE_TOPOLOGY_TO_VK_PRIMITIVE_TOPOLOGY_MAPPING,
// REVERSE_MAPPING)

CREATE_MAPPER_CPP(CullMode, VkCullModeFlags, CULL_MODE_TO_VK_CULL_MODE_MAPPING, FORWARD_MAPPING)
// CREATE_MAPPER_CPP(VkCullModeFlags, CullMode, CULL_MODE_TO_VK_CULL_MODE_MAPPING, REVERSE_MAPPING)

CREATE_MAPPER_CPP(FrontFace, VkFrontFace, FRONT_FACE_TO_VK_FRONT_FACE_MAPPING, FORWARD_MAPPING)
// CREATE_MAPPER_CPP(VkFrontFace, FrontFace, FRONT_FACE_TO_VK_FRONT_FACE_MAPPING, REVERSE_MAPPING)

CREATE_MAPPER_CPP(BufferUsageRate,
  VkVertexInputRate,
  BUFFER_USAGE_RATE_TO_VK_VERTEX_INPUT_RATE_MAPPING,
  FORWARD_MAPPING)
// CREATE_MAPPER_CPP(VkFrontFace, FrontFace, FRONT_FACE_TO_VK_FRONT_FACE_MAPPING, REVERSE_MAPPING)

CREATE_MAPPER_CPP(ImageType, VkImageType, IMAGE_TYPE_TO_VK_IMAGE_TYPE, FORWARD_MAPPING);
// CREATE_MAPPER_CPP(ImageType, VkImageType, IMAGE_TYPE_TO_VK_IMAGE_TYPE, REVERSE_MAPPING)

VkShaderStageFlagBits GetCombinedShaderStageFlag(ShaderStage stage) {
  if (ENUM_HAS(stage, ShaderStage::All)) {
    return ToVkShaderStageFlagBits(ShaderStage::All).value();
  }

  U32 result = 0;

  if (ENUM_HAS(stage, ShaderStage::Vertex)) {
    result = result | ToVkShaderStageFlagBits(ShaderStage::Vertex).value();
  }

  if (ENUM_HAS(stage, ShaderStage::Pixel)) {
    result = result | ToVkShaderStageFlagBits(ShaderStage::Pixel).value();
  }

  if (ENUM_HAS(stage, ShaderStage::Compute)) {
    result = result | ToVkShaderStageFlagBits(ShaderStage::Compute).value();
  }

  if (ENUM_HAS(stage, ShaderStage::Geometry)) {
    result = result | ToVkShaderStageFlagBits(ShaderStage::Geometry).value();
  }

  // TODO(vasumahesh1):[ENUM]: Check safety
  return static_cast<VkShaderStageFlagBits>(result);
}

} // namespace Vulkan
} // namespace Azura
