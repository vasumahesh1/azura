#pragma once

#include <vulkan/vulkan_core.h>
#include "Types.h"
#include "Log/Log.h"
#include "Generic/TextureManager.h"

namespace Azura {
namespace Vulkan {

struct ImageTransition {
  VkImageLayout m_layout;
  VkAccessFlags m_accessMask;
  VkPipelineStageFlags m_stageMask;
};

class VkScopedImage {
public:
  VkScopedImage(VkDevice device,
                const TextureDesc& textureDesc,
                VkImageUsageFlags usage,
                const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties,
                Log logger);

  explicit VkScopedImage(Log logger);

  void Create(VkDevice device,
              const TextureDesc& textureDesc,
              VkImageUsageFlags usage,
              const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties);

  VkImage Real() const;
  VkImageView View() const;
  VkDeviceMemory Memory() const;

  void CleanUp() const;

  void TransitionLayout(VkCommandBuffer commandBuffer,
                        ImageTransition oldTransition,
                        ImageTransition newTransition) const;
  void CopyFromBuffer(VkCommandBuffer commandBuffer, const TextureBufferInfo& bufferInfo, VkBuffer buffer) const;

  void CreateImageView(ImageViewType imageView);

private:
  VkDevice m_device;

  VkImage m_image{};
  VkImageView m_imageView{};
  VkDeviceMemory m_memory{};

  TextureDesc m_desc;

  const Log log_VulkanRenderSystem;
};

} // namespace Vulkan
} // namespace Azura
