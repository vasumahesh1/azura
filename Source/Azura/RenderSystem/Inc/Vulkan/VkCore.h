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

VkRenderPass CreateRenderPass(VkDevice device, VkFormat colorFormat);

void BindUniformBufferToDescriptorSet(std::vector<VkDescriptorSetLayoutBinding>& bindings, VkShaderStageFlags stageFlag);
VkDescriptorSetLayout CreateDescriptorSetLayout(VkDevice device, const std::vector<VkDescriptorSetLayoutBinding>& bindings);
VkPipelineLayout CreateGraphicsPipelineLayout(VkDevice device, const std::vector<VkDescriptorSetLayout>& descriptorSets);

VkShaderModule CreateShaderModule(VkDevice device, const std::vector<U8>& code);
};
} // namespace Vulkan
} // namespace Azura
