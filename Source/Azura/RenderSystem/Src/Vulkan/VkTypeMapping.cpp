#include "Vulkan/VkTypeMapping.h"

namespace Azura {
namespace Vulkan {

#define RAW_STORAGE_TO_VK_FORMAT_MAPPING(FUNC) \
  FUNC(RawStorageFormat::UNKNOWN, VK_FORMAT_UNDEFINED) \
  FUNC(RawStorageFormat::R32G32B32A32_FLOAT, VK_FORMAT_R32G32B32A32_SFLOAT)

#define COLOR_SPACE_TO_VK_COLOR_SPACE_MAPPING(FUNC) \
  FUNC(ColorSpace::SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) \
  FUNC(ColorSpace::HDR10, VK_COLOR_SPACE_HDR10_HLG_EXT)

#define PRESENT_MODES_TO_VK_PRESENT_MODE_MAPPING(FUNC) \
  FUNC(PresentModes::Immediate, VK_PRESENT_MODE_IMMEDIATE_KHR) \
  FUNC(PresentModes::Mailbox, VK_PRESENT_MODE_MAILBOX_KHR) \
  FUNC(PresentModes::FIFO, VK_PRESENT_MODE_FIFO_KHR) \
  FUNC(PresentModes::FIFORelaxed, VK_PRESENT_MODE_FIFO_RELAXED_KHR) \
  FUNC(PresentModes::SharedDemandRefesh, VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR) \
  FUNC(PresentModes::SharedContinuous, VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR) 


#define SHADER_STAGE_TO_SHADER_STAGE_FLAGS_MAPPING(FUNC) \
  FUNC(ShaderStage::All, VK_SHADER_STAGE_ALL) \
  FUNC(ShaderStage::Vertex, VK_SHADER_STAGE_VERTEX_BIT) \
  FUNC(ShaderStage::Pixel, VK_SHADER_STAGE_FRAGMENT_BIT) \
  FUNC(ShaderStage::Compute, VK_SHADER_STAGE_COMPUTE_BIT) \
  FUNC(ShaderStage::Geometry, VK_SHADER_STAGE_GEOMETRY_BIT) 


#define PRIMITIVE_TOPOLOGY_TO_VK_PRIMITIVE_TOPOLOGY_MAPPING(FUNC) \
  FUNC(PrimitiveTopology::PointList, VK_PRIMITIVE_TOPOLOGY_POINT_LIST) \
  FUNC(PrimitiveTopology::LineList, VK_PRIMITIVE_TOPOLOGY_LINE_LIST) \
  FUNC(PrimitiveTopology::LineStrip, VK_PRIMITIVE_TOPOLOGY_LINE_STRIP) \
  FUNC(PrimitiveTopology::TriangleList, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST) \
  FUNC(PrimitiveTopology::TriangleStrip, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP) \
  FUNC(PrimitiveTopology::TriangleFan, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN) \
  FUNC(PrimitiveTopology::LineListWithAdjacency, VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY) \
  FUNC(PrimitiveTopology::LineStripWithAdjacency, VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY) \
  FUNC(PrimitiveTopology::TriangleListWithAdjacency, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY) \
  FUNC(PrimitiveTopology::TriangleStripWithAdjacency, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY) \
  FUNC(PrimitiveTopology::PatchList, VK_PRIMITIVE_TOPOLOGY_PATCH_LIST) 


#define CULL_MODE_TO_VK_CULL_MODE_MAPPING(FUNC) \
  FUNC(CullMode::None, VK_CULL_MODE_NONE) \
  FUNC(CullMode::FrontBit, VK_CULL_MODE_FRONT_BIT) \
  FUNC(CullMode::BackBit, VK_CULL_MODE_BACK_BIT) \
  FUNC(CullMode::FrontAndBack, VK_CULL_MODE_FRONT_AND_BACK) 


#define FRONT_FACE_TO_VK_FRONT_FACE_MAPPING(FUNC) \
  FUNC(FrontFace::CounterClockwise, VK_FRONT_FACE_COUNTER_CLOCKWISE) \
  FUNC(FrontFace::Clockwise, VK_FRONT_FACE_CLOCKWISE) 


#define FORWARD_MAPPING(TYPE_A, TYPE_B)    \
  case TYPE_A:                          \
    return TYPE_B;

#define REVERSE_MAPPING(TYPE_A, TYPE_B)    \
  case TYPE_B:                          \
    return TYPE_A;

#define CREATE_MAPPER_CPP(FROM_FORMAT, TO_FORMAT, MAPPING_TABLE, CASE_MAPPING_FUNC)                \
  CREATE_MAPPER_H(FROM_FORMAT, TO_FORMAT) {                                                                  \
  switch(inputFormat) {                                                     \
  MAPPING_TABLE(CASE_MAPPING_FUNC)                                               \
  }                                                                         \
  return std::nullopt;                                                      \
  }

CREATE_MAPPER_CPP(RawStorageFormat, VkFormat, RAW_STORAGE_TO_VK_FORMAT_MAPPING, FORWARD_MAPPING)
// CREATE_MAPPER_CPP(VkFormat, RawStorageFormat, RAW_STORAGE_TO_VK_FORMAT_MAPPING, REVERSE_MAPPING)

CREATE_MAPPER_CPP(ColorSpace, VkColorSpaceKHR, COLOR_SPACE_TO_VK_COLOR_SPACE_MAPPING, FORWARD_MAPPING)
// CREATE_MAPPER_CPP(VkColorSpaceKHR, ColorSpace, COLOR_SPACE_TO_VK_COLOR_SPACE_MAPPING, REVERSE_MAPPING)

CREATE_MAPPER_CPP(PresentModes, VkPresentModeKHR, PRESENT_MODES_TO_VK_PRESENT_MODE_MAPPING, FORWARD_MAPPING)
// CREATE_MAPPER_CPP(VkPresentModeKHR, PresentModes, PRESENT_MODES_TO_VK_PRESENT_MODE_MAPPING, REVERSE_MAPPING)

CREATE_MAPPER_CPP(ShaderStage, VkShaderStageFlagBits, SHADER_STAGE_TO_SHADER_STAGE_FLAGS_MAPPING, FORWARD_MAPPING)
// CREATE_MAPPER_CPP(VkShaderStageFlagBits, ShaderStage, SHADER_STAGE_TO_SHADER_STAGE_FLAGS_MAPPING, REVERSE_MAPPING)

CREATE_MAPPER_CPP(PrimitiveTopology, VkPrimitiveTopology, PRIMITIVE_TOPOLOGY_TO_VK_PRIMITIVE_TOPOLOGY_MAPPING, FORWARD_MAPPING)
// CREATE_MAPPER_CPP(VkPrimitiveTopology, PrimitiveTopology, PRIMITIVE_TOPOLOGY_TO_VK_PRIMITIVE_TOPOLOGY_MAPPING, REVERSE_MAPPING)

CREATE_MAPPER_CPP(CullMode, VkCullModeFlags, CULL_MODE_TO_VK_CULL_MODE_MAPPING, FORWARD_MAPPING)
// CREATE_MAPPER_CPP(VkCullModeFlags, CullMode, CULL_MODE_TO_VK_CULL_MODE_MAPPING, REVERSE_MAPPING)

CREATE_MAPPER_CPP(FrontFace, VkFrontFace, FRONT_FACE_TO_VK_FRONT_FACE_MAPPING, FORWARD_MAPPING)
// CREATE_MAPPER_CPP(VkFrontFace, FrontFace, FRONT_FACE_TO_VK_FRONT_FACE_MAPPING, REVERSE_MAPPING)

} // namespace Vulkan
} // namespace Azura
