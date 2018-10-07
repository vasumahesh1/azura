#include <utility>
#include "Vulkan/VkScopedImage.h"
#include "Vulkan/VkTypeMapping.h"
#include "Vulkan/VkTextureManager.h"
#include "Vulkan/VkCore.h"
#include "Vulkan/VkMacros.h"
#include "Memory/MemoryFactory.h"

namespace Azura {
namespace Vulkan {
VkScopedImage::VkScopedImage(VkDevice device,
                             const TextureDesc& textureDesc,
                             VkImageUsageFlags usage,
                             const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties,
                             Log logger)
  : m_device(device),
    m_desc(textureDesc),
    log_VulkanRenderSystem(std::move(logger)) {
  Create(m_device, textureDesc, usage, physicalDeviceMemoryProperties);
}

VkScopedImage::VkScopedImage(VkDevice device, const TextureDesc& textureDesc, VkImage image, Log logger) 
  : m_device(device),
    m_image(image),
    m_desc(textureDesc),
    log_VulkanRenderSystem(std::move(logger)) {
}

VkScopedImage::VkScopedImage(Log logger)
  : m_device(),
    log_VulkanRenderSystem(std::move(logger)) {
}

void VkScopedImage::Create(VkDevice device,
                           const TextureDesc& textureDesc,
                           VkImageUsageFlags usage,
                           const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties) {

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

VkImageView VkScopedImage::View() const {
  return m_imageView;
}

VkDeviceMemory VkScopedImage::Memory() const {
  return m_memory;
}

void VkScopedImage::CleanUp() const {
  vkDestroyImage(m_device, m_image, nullptr);
  vkFreeMemory(m_device, m_memory, nullptr);
  vkDestroyImageView(m_device, m_imageView, nullptr);
}

void VkScopedImage::TransitionLayout(VkCommandBuffer commandBuffer,
                                     ImageTransition oldTransition,
                                     ImageTransition newTransition) const {
  // TODO(vasumahesh1):[TEXTURE]: Add support for Depth and Stencil related Aspects
  const VkImageSubresourceRange resourceRange = {
    VK_IMAGE_ASPECT_COLOR_BIT, 0, m_desc.m_mipLevels, 0, m_desc.m_arrayLayers
  };
  VkCore::TransitionImageLayout(commandBuffer, m_image, oldTransition.m_accessMask, newTransition.m_accessMask,
                                oldTransition.m_layout, newTransition.m_layout, oldTransition.m_stageMask,
                                newTransition.m_stageMask, resourceRange);

}

void VkScopedImage::CopyFromBuffer(VkCommandBuffer commandBuffer,
                                   const TextureBufferInfo& bufferInfo,
                                   VkBuffer buffer) const {
  // TODO(vasumahesh1):[TEXTURE]: Add support for Depth and Stencil related Aspects
  U32 currentWidth  = m_desc.m_bounds.m_width;
  U32 currentHeight = m_desc.m_bounds.m_height;
  U32 currentDepth  = m_desc.m_bounds.m_depth;

  VkBufferImageCopy region               = {};
  region.bufferOffset                    = bufferInfo.m_offset;
  region.bufferRowLength                 = 0;
  region.bufferImageHeight               = 0;
  region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel       = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount     = 1;

  region.imageOffset = {0, 0, 0};
  region.imageExtent = {
    currentWidth,
    currentHeight,
    currentDepth
  };

  vkCmdCopyBufferToImage(
                         commandBuffer,
                         buffer,
                         m_image,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                         1,
                         &region);
}

void VkScopedImage::CreateImageView(ImageViewType imageView) {
  const auto vkImageView = ToVkImageViewType(imageView);
  VERIFY_OPT(log_VulkanRenderSystem, vkImageView, "Unknown VkImageViewType");

  const auto vkFormat = ToVkFormat(m_desc.m_format);
  VERIFY_OPT(log_VulkanRenderSystem, vkFormat, "Unknown VkFormat");

  // TODO(vasumahesh1):[TEXTURE]: Add support for Depth and Stencil related Aspects
  VkImageViewCreateInfo viewInfo           = {};
  viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image                           = m_image;
  viewInfo.viewType                        = vkImageView.value();
  viewInfo.format                          = vkFormat.value();
  viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
  viewInfo.subresourceRange.baseMipLevel   = 0;
  viewInfo.subresourceRange.levelCount     = m_desc.m_mipLevels;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount     = m_desc.m_arrayLayers;

  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateImageView(m_device, &viewInfo, nullptr, &m_imageView),
    "Failed to create texture image view");
}

} // namespace Vulkan
} // namespace Azura
