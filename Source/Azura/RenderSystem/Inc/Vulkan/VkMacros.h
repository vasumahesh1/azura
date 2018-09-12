#pragma once

#include <vulkan/vulkan_core.h>
#include <stdexcept>

#include "VkDebug.h"
#include "Log/Log.h"

namespace Azura {
namespace Vulkan {
#define VERIFY_VK_OP(Logger, res, message)                                                           \
  if ((res) != VK_SUCCESS) {                                                                         \
    LOG_ERR(Logger, LOG_LEVEL, message);                                                             \
    LOG_ERR(Logger, LOG_LEVEL, VkResultToString(res));                                               \
    throw std::runtime_error(message);                                                               \
  }

#define VK_KHR_SURFACE "VK_KHR_surface"

} // namespace Vulkan
} // namespace Azura
