#pragma once

#include <vulkan/vulkan_core.h>

#include "Generic/Window.h"
#include "Containers/Vector.h"

namespace Azura {
namespace Vulkan {

class VkWindow final : public Window {
public:
  VkWindow(void* windowResource, U32 width, U32 height);

  VkSurfaceKHR CreateSurface(VkInstance instance) const;

  static void GetInstanceExtensions(Containers::Vector<const char*>& extensions);
};
} // namespace Vulkan
} // namespace Azura
