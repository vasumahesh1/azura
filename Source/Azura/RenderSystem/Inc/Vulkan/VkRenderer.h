#pragma once

#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <array>
#include <vector>

#include "Containers/Vector.h"
#include "Generic/Renderer.h"
#include "VkCore.h"

namespace Azura {
namespace Vulkan {
class VulkanRenderer : public Renderer {
public:
  VulkanRenderer(const ApplicationInfo& appInfo, const DeviceRequirements &deviceRequirements);
  ~VulkanRenderer();

  void AddRenderEntity() override;
};
} // namespace Vulkan
} // namespace Azura

