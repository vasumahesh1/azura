#pragma once
#include <vulkan/vulkan_core.h>
#include <array>
#include <vector>
#include "Containers/Vector.h"
#include "Types.h"

namespace Azura {
namespace Vulkan {
struct VkQueueIndices {
  int m_graphicsFamily{-1};
  int m_presentFamily{-1};

  // Special Transfer Queue for copying buffer data from
  // GPU (Host Accessible) to a GPU Optimized location that
  // may or may not be accessible by the host.
  int m_transferFamily{-1};

  bool m_isTransferQueueRequired;

  bool IsComplete() const;

  std::array<U32, 2> GetIndicesArray() const;
  U32 GetActiveSize() const;
};

struct SwapChainDeviceSupport {
  VkSurfaceCapabilitiesKHR m_capabilities{};
  Containers::Vector<VkSurfaceFormatKHR> m_formats;
  Containers::Vector<VkPresentModeKHR> m_presentModes;

  explicit SwapChainDeviceSupport(Memory::Allocator& allocator);
  bool IsSupported() const;
};

struct VkScopedSwapChain {
  VkSwapchainKHR m_swapChain;
  VkExtent2D m_extent;
  VkSurfaceFormatKHR m_surfaceFormat;
  U32 m_imageCount{0};
  Containers::Vector<VkImage> m_images;

  Containers::Vector<VkImageView> m_imageViews;

  VkScopedSwapChain(Memory::Allocator& allocator);

  void CleanUp(VkDevice device);
};
}  // namespace Vulkan
}  // namespace Azura