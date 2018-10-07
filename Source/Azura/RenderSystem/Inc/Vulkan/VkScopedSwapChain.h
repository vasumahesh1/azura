#pragma once

#include <vulkan/vulkan_core.h>
#include "Types.h"
#include <Containers/Vector.h>
#include "VkScopedImage.h"


namespace Azura {
struct SwapChainRequirements;
}

namespace Azura {
namespace Vulkan {
struct VkQueueIndices;
struct SwapChainDeviceSupport;

class VkScopedSwapChain {
public:
  VkScopedSwapChain(Memory::Allocator& allocator, Log logger);

  void Create(VkDevice device,
              VkSurfaceKHR surface,
              const VkQueueIndices& queueIndices,
              const SwapChainDeviceSupport& swapChainSupport,
              const SwapChainRequirements& swapChainRequirement);

  void CleanUp(VkDevice device);
  VkSwapchainKHR Real() const;

  VkFormat GetSurfaceFormat() const;
  const VkExtent2D& GetExtent() const;
  const VkScopedImage& GetImage(int frameIdx) const;

  const Containers::Vector<VkScopedImage>& GetAllImages() const;

private:
  const Log log_VulkanRenderSystem;

  VkSwapchainKHR m_swapChain;
  VkExtent2D m_extent;
  VkSurfaceFormatKHR m_surfaceFormat;
  U32 m_imageCount{0};

  Containers::Vector<VkScopedImage> m_images;

  // Containers::Vector<VkImage> m_images;
  // Containers::Vector<VkImageView> m_imageViews;
};

} // namespace Vulkan
} // namespace Azura
