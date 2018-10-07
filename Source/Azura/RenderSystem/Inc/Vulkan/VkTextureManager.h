#pragma once
#include "Generic/TextureManager.h"
#include <vulkan/vulkan_core.h>
#include "Generic/Renderer.h"

namespace Azura {
namespace Vulkan {

class VkTextureManager : public TextureManager
{
public:
  VkTextureManager(VkDevice device, const TextureRequirements& requirements, Log log);

private:
  VkDevice m_device;

  Log log_VulkanRenderSystem;
};

} // namespace Vulkan
} // namespace Azura