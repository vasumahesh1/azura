#pragma once

#include <map>
#include "Generic/GenericTypes.h"
#include <vulkan/vulkan_core.h>
#include <optional>
#include "Generic/RawStorageFormat.h"

namespace Azura {
namespace Vulkan {
namespace Impl {
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

} // namespace Vulkan
} // namespace Azura
