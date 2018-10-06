#pragma once

#include <vulkan/vulkan_core.h>
#include "Types.h"
#include "Log/Log.h"

namespace Azura {
struct TextureDesc;
}

namespace Azura {
namespace Vulkan {
class VkScopedImage {
public:
  VkScopedImage(VkDevice device, const TextureDesc& textureDesc, VkImageUsageFlagBits usage, const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties, Log logger);

  VkImage Real() const;
  VkDeviceMemory Memory() const;

  void CleanUp() const;

private:
  VkDevice m_device;

  VkImage m_image;
  VkDeviceMemory m_memory{};

  const Log log_VulkanRenderSystem;
};

} // namespace Vulkan
} // namespace Azura
