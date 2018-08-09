#pragma once

#include <vulkan/vulkan_core.h>

#include "Generic/Renderer.h"
#include "Types.h"

namespace Azura {
struct VkQueueIndices {
  int m_graphicsFamily{-1};
  int m_presentFamily{-1};

  // Special Transfer Queue for copying buffer data from
  // GPU (Host Accessible) to a GPU Optimized location that
  // may or may not be accessible by the host.
  int m_transferFamily{-1};

  bool IsComplete(bool isTransferQueueRequired) const;
};

struct SwapChainSupportDetails
{
  VkSurfaceCapabilitiesKHR m_capabilities;
  std::vector<VkSurfaceFormatKHR> m_formats;
  std::vector<VkPresentModeKHR> m_presentModes;
};

namespace VulkanCore {
VkInstance CreateInstance(const ApplicationInfo& applicationData, std::vector<const char*> vkExtensions);

VkPhysicalDevice SelectPhysicalDevice(VkInstance instance,
                                      VkSurfaceKHR surface,
                                      const DeviceRequirements& requirements);
VkDevice CreateLogicalDevice(VkPhysicalDevice physicalDevice, const VkQueueIndices& queueIndices, const DeviceRequirements& requirements);

VkQueue GetQueueFromDevice(VkDevice device, int queueIndex);

void CreateSwapChain();
void CreateRenderPass();
void CreateDescriptorSetLayout();
void CreateGraphicsPipelineLayout();
void CreateShaderModule(const std::vector<U8>& code);
};
}
