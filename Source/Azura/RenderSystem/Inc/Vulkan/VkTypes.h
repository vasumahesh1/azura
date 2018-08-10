#pragma once
#include <array>
#include <vector>
#include "Types.h"
#include <vulkan/vulkan_core.h>

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
  std::vector<VkSurfaceFormatKHR> m_formats;
  std::vector<VkPresentModeKHR> m_presentModes;

  bool IsSupported() const;
};

struct SwapChainCreateResult {
  VkSwapchainKHR m_swapChain;
  VkExtent2D m_extent;
  VkSurfaceFormatKHR m_surfaceFormat;
  U32 m_imageCount{0};
};
} // namespace Vulkan
} // namespace Azura