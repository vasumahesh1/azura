#pragma once

#include <map>
#include "Generic/GenericTypes.h"
#include <vulkan/vulkan_core.h>
#include <optional>
#include "Generic/RawStorageFormat.h"

namespace Azura {
namespace Vulkan {
namespace Impl {
// TODO(vasumahesh1): THIS CODE IS TOOO SLOW. Must use switch cases and a smart macro

const std::map<RawStorageFormat, VkFormat> VK_FORMAT_MAPPING = {
  std::make_pair(RawStorageFormat::AZ_UNKNOWN, VK_FORMAT_UNDEFINED),
  std::make_pair(RawStorageFormat::AZ_R32G32B32A32_FLOAT, VK_FORMAT_R32G32B32A32_SFLOAT),
};

const std::map<ColorSpace, VkColorSpaceKHR> VK_COLOR_SPACE_MAPPING = {
  std::make_pair(ColorSpace::SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR),
  std::make_pair(ColorSpace::HDR10, VK_COLOR_SPACE_HDR10_HLG_EXT),
};

const std::map<PresentModes, VkPresentModeKHR> VK_PRESENT_MODE_MAPPING = {
  std::make_pair(PresentModes::Immediate, VK_PRESENT_MODE_IMMEDIATE_KHR),
  std::make_pair(PresentModes::Mailbox, VK_PRESENT_MODE_MAILBOX_KHR),
  std::make_pair(PresentModes::FIFO, VK_PRESENT_MODE_FIFO_KHR),
  std::make_pair(PresentModes::FIFORelaxed, VK_PRESENT_MODE_FIFO_RELAXED_KHR),
  std::make_pair(PresentModes::SharedDemandRefesh, VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR),
  std::make_pair(PresentModes::SharedContinuous, VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR),
};

const std::map<ShaderStage, VkShaderStageFlagBits> VK_SHADER_STAGE_MAPPING = {
  std::make_pair(ShaderStage::All, VK_SHADER_STAGE_ALL),
  std::make_pair(ShaderStage::Vertex, VK_SHADER_STAGE_VERTEX_BIT),
  std::make_pair(ShaderStage::Pixel, VK_SHADER_STAGE_FRAGMENT_BIT),
  std::make_pair(ShaderStage::Compute, VK_SHADER_STAGE_COMPUTE_BIT),
  std::make_pair(ShaderStage::Geometry, VK_SHADER_STAGE_GEOMETRY_BIT),
};

const std::map<PrimitiveTopology, VkPrimitiveTopology> VK_PRIMITIVE_TOPOLOGY_MAPPING = {
  std::make_pair(PrimitiveTopology::PointList, VK_PRIMITIVE_TOPOLOGY_POINT_LIST),
  std::make_pair(PrimitiveTopology::LineList, VK_PRIMITIVE_TOPOLOGY_LINE_LIST),
  std::make_pair(PrimitiveTopology::LineStrip, VK_PRIMITIVE_TOPOLOGY_LINE_STRIP),
  std::make_pair(PrimitiveTopology::TriangleList, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST),
  std::make_pair(PrimitiveTopology::TriangleStrip, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP),
  std::make_pair(PrimitiveTopology::TriangleFan, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN),
  std::make_pair(PrimitiveTopology::LineListWithAdjacency, VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY),
  std::make_pair(PrimitiveTopology::LineStripWithAdjacency, VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY),
  std::make_pair(PrimitiveTopology::TriangleListWithAdjacency, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY),
  std::make_pair(PrimitiveTopology::TriangleStripWithAdjacency, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY),
  std::make_pair(PrimitiveTopology::PatchList, VK_PRIMITIVE_TOPOLOGY_PATCH_LIST),
};

const std::map<CullMode, VkCullModeFlags> VK_CULL_MODE_MAPPING = {
  std::make_pair(CullMode::None, VK_CULL_MODE_NONE),
  std::make_pair(CullMode::FrontBit, VK_CULL_MODE_FRONT_BIT),
  std::make_pair(CullMode::BackBit, VK_CULL_MODE_BACK_BIT),
  std::make_pair(CullMode::FrontAndBack, VK_CULL_MODE_FRONT_AND_BACK),
};

const std::map<FrontFace, VkFrontFace> VK_FRONT_FACE_MAPPING = {
  std::make_pair(FrontFace::CounterClockwise, VK_FRONT_FACE_COUNTER_CLOCKWISE),
  std::make_pair(FrontFace::Clockwise, VK_FRONT_FACE_CLOCKWISE),
};

template<typename FromType, typename ToType>
std::optional<ToType> ConvertFormat(FromType fromValue, const std::map<FromType, ToType> sourceMap) {
  const auto itr = sourceMap.find(fromValue);

  if (itr == sourceMap.end()) {
    return std::nullopt;
  }

  return (*itr).second;
}
} // namespace Impl

inline auto ToVkFormat(const RawStorageFormat format)
{
  return Impl::ConvertFormat<RawStorageFormat, VkFormat>(format, Impl::VK_FORMAT_MAPPING);
}

inline auto ToVkColorSpaceKHR(const ColorSpace colorSpace)
{
  return Impl::ConvertFormat<ColorSpace, VkColorSpaceKHR>(colorSpace, Impl::VK_COLOR_SPACE_MAPPING);
}

inline auto ToVkPresentModeKHR(const PresentModes presentMode)
{
  return Impl::ConvertFormat<PresentModes, VkPresentModeKHR>(presentMode, Impl::VK_PRESENT_MODE_MAPPING);
}

inline auto ToVkShaderStageFlags(const ShaderStage shaderStage)
{
  return Impl::ConvertFormat<ShaderStage, VkShaderStageFlagBits>(shaderStage, Impl::VK_SHADER_STAGE_MAPPING);
}

inline auto ToVkPrimitiveTopology(const PrimitiveTopology topology)
{
  return Impl::ConvertFormat<PrimitiveTopology, VkPrimitiveTopology>(topology, Impl::VK_PRIMITIVE_TOPOLOGY_MAPPING);
}

inline auto ToVkCullModeFlags(const CullMode mode)
{
  return Impl::ConvertFormat<CullMode, VkCullModeFlags>(mode, Impl::VK_CULL_MODE_MAPPING);
}

inline auto ToVkFrontFace(const FrontFace face)
{
  return Impl::ConvertFormat<FrontFace, VkFrontFace>(face, Impl::VK_FRONT_FACE_MAPPING);
}

} // namespace Vulkan
} // namespace Azura
