#pragma once

#include <vulkan/vulkan_core.h>
#include "Containers/Vector.h"

namespace Azura {
namespace Vulkan {

namespace VkPlatform {
VkSurfaceKHR CreateSurface(const void* windowHandle, VkInstance instance);
void GetInstanceExtensions(Containers::Vector<const char*>& extensions);
}  // namespace VkPlatform

}  // namespace Vulkan
}  // namespace Azura
