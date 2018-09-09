#pragma once

#include <vulkan/vulkan_core.h>
#include <stdexcept>

#include "VkDebug.h"

namespace Azura {
namespace Vulkan {
#define VERIFY_VK_OP(res, message)                                                                                     \
  if ((res) != VK_SUCCESS) {                                                                                           \
    throw std::runtime_error(VkResultToString(res));                                                                                \
  }

#define VERIFY_TRUE(res, message)                                                                                      \
  if ((res) != true) {                                                                                                 \
    throw std::runtime_error(message);                                                                                \
  }

#define VERIFY_OPT(res, message)                                                                                       \
  if (!(res)) {                                                                                                         \
    throw std::runtime_error(message);                                                                                \
  }

#define FAIL_IF(res, message)                                                                                       \
  if ((res)) {                                                                                                         \
    throw std::runtime_error(message);                                                                                \
  }

#define VK_KHR_SURFACE "VK_KHR_surface"

}  // namespace Vulkan
}  // namespace Azura