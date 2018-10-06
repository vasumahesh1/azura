#include <utility>
#include "Vulkan/VkScopedImage.h"
#include "Vulkan/VkTypeMapping.h"
#include "Vulkan/VkTextureManager.h"
#include "Vulkan/VkCore.h"
#include "Vulkan/VkMacros.h"

namespace Azura {
namespace Vulkan {
VkScopedImage::VkScopedImage(VkDevice device,
                             const TextureDesc& textureDesc,
                             VkImageUsageFlagBits usage,
                             const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties,
                             Log logger)
  : m_device(device),
    log_VulkanRenderSystem(std::move(logger)) {

  m_image = VkCore::CreateImage(device, textureDesc.m_format, textureDesc.m_type,
                                {textureDesc.m_bounds.m_width, textureDesc.m_bounds.m_height},
                                textureDesc.m_bounds.m_depth, textureDesc.m_arrayLayers, textureDesc.m_mipLevels,
                                VK_IMAGE_TILING_OPTIMAL, usage, log_VulkanRenderSystem);

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(m_device, m_image, &memRequirements);

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize       = memRequirements.size;
  allocInfo.memoryTypeIndex      = VkCore::FindMemoryType(memRequirements.memoryTypeBits,
                                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                          physicalDeviceMemoryProperties);

  VERIFY_VK_OP(log_VulkanRenderSystem, vkAllocateMemory(m_device, &allocInfo, nullptr, &m_memory),
    "Failed to allocate memory for image texture");
  vkBindImageMemory(m_device, m_image, m_memory, 0);
}

VkImage VkScopedImage::Real() const {
  return m_image;
}

VkDeviceMemory VkScopedImage::Memory() const {
  return m_memory;
}

void VkScopedImage::CleanUp() const {
  vkDestroyImage(m_device, m_image, nullptr);
  vkFreeMemory(m_device, m_memory, nullptr);
}
} // namespace Vulkan
} // namespace Azura
