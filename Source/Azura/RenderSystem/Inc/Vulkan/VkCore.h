#pragma once

#include <vulkan/vulkan_core.h>

#include "Generic/Renderer.h"
#include "Types.h"
#include "VkTypes.h"

namespace Azura {
namespace Vulkan {
namespace VkCore {

VkInstance CreateInstance(const ApplicationInfo& applicationData, std::vector<const char*> vkExtensions);

SwapChainDeviceSupport QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

VkPhysicalDevice SelectPhysicalDevice(VkInstance instance,
                                      VkSurfaceKHR surface,
                                      const DeviceRequirements& requirements,
                                      const SwapChainDeviceSupport& swapChainSupport);
VkDevice CreateLogicalDevice(VkPhysicalDevice physicalDevice,
                             const VkQueueIndices& queueIndices,
                             const DeviceRequirements& requirements);

VkQueueIndices FindQueueFamiliesInDevice(VkPhysicalDevice device,
                                         VkSurfaceKHR surface,
                                         const DeviceRequirements& requirements);

VkQueue GetQueueFromDevice(VkDevice device, int queueIndex);

SwapChainCreateResult CreateSwapChain(VkDevice device,
                                      VkSurfaceKHR surface,
                                      const VkQueueIndices& queueIndices,
                                      const SwapChainDeviceSupport& swapChainSupport,
                                      const SwapChainRequirement& swapChainRequirement);


std::vector<VkImage> GetSwapChainImages(VkDevice device, VkSwapchainKHR swapChain, U32 imageCount);

void CreateRenderPass();
void CreateDescriptorSetLayout();
void CreateGraphicsPipelineLayout();
void CreateShaderModule(const std::vector<U8>& code);
};
} // namespace Vulkan
} // namespace Azura
