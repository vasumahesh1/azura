#pragma once
#include <vulkan/vulkan_core.h>

namespace Azura {
namespace Vulkan {

/**
 * \brief Converts VkResult to console printable string
 * \param err VkResult Error
 * \return String converted for debug
 */
const char* VkResultToString(const VkResult err);
const char* VkPhysicalDeviceTypeToString(const VkPhysicalDeviceType deviceType);
} // namespace Vulkan
} // namespace Azura
