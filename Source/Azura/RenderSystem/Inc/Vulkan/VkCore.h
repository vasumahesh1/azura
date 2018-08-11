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

VkScopedSwapChain CreateSwapChain(VkDevice device,
                                  VkSurfaceKHR surface,
                                  const VkQueueIndices& queueIndices,
                                  const SwapChainDeviceSupport& swapChainSupport,
                                  const SwapChainRequirement& swapChainRequirement);

VkImageView CreateImageView(VkDevice device,
                            VkImage sourceImage,
                            VkImageViewType viewType,
                            VkFormat viewFormat,
                            VkImageAspectFlags aspectMask,
                            U32 baseMip = 0,
                            U32 levelCount = 1,
                            U32 baseArrayLayer = 0,
                            U32 layerCount = 1);


VkRenderPass CreateRenderPass(VkDevice device, VkFormat colorFormat);

void CreateUniformBufferBinding(std::vector<VkDescriptorSetLayoutBinding>& bindings,
                                      VkShaderStageFlags stageFlag);
VkDescriptorSetLayout CreateDescriptorSetLayout(VkDevice device,
                                                const std::vector<VkDescriptorSetLayoutBinding>& bindings);
VkPipelineLayout CreatePipelineLayout(VkDevice device, const std::vector<VkDescriptorSetLayout>& descriptorSets);

VkShaderModule CreateShaderModule(VkDevice device, const std::vector<U8>& code);

std::vector<VkFramebuffer> CreateFrameBuffers(VkDevice device,
                                              VkRenderPass renderPass,
                                              const VkScopedSwapChain& scopedSwapChain);

VkCommandPool CreateCommandPool(VkDevice device, int queueIndex);

uint32_t FindMemoryType(uint32_t typeFilter,
                        VkMemoryPropertyFlags properties,
                        const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties);

VkDescriptorPoolSize CreateDescriptorPoolSize(VkDescriptorType type, U32 descriptorCount);

VkDescriptorPool CreateDescriptorPool(VkDevice device, const std::vector<VkDescriptorPoolSize>& pools, U32 maxSets);

VkDescriptorSet CreateDescriptorSet(VkDevice device,
                                    VkDescriptorPool descriptorPool,
                                    const std::vector<VkDescriptorSetLayout>& descriptorSets);

VkWriteDescriptorSet CreateWriteDescriptorForUniformBuffer(VkDescriptorSet set,
                                                           U32 layoutIndex,
                                                           U32 binding,
                                                           const std::vector<VkDescriptorBufferInfo>& bufferInfos);

void UpdateDescriptorSets(VkDevice device, const std::vector<VkWriteDescriptorSet>& descriptorWrites);

VkCommandBuffer CreateCommandBuffer(VkDevice device, VkCommandPool commandPool, VkCommandBufferLevel level);

std::vector<VkCommandBuffer> CreateCommandBuffers(VkDevice device,
                                                  U32 count,
                                                  VkCommandPool commandPool,
                                                  VkCommandBufferLevel level);

void CreateCommandBuffers(VkDevice device,
                          U32 count,
                          VkCommandPool commandPool,
                          VkCommandBufferLevel level,
                          std::vector<VkCommandBuffer>&
                          commandBuffers);

void BeginCommandBuffer(VkCommandBuffer buffer, VkCommandBufferUsageFlags flags);

void EndCommandBuffer(VkCommandBuffer buffer);

VkSemaphore CreateSemaphore(VkDevice device);

void CreateSemaphores(VkDevice device, U32 count, std::vector<VkSemaphore>& semaphores);

std::vector<VkSemaphore> CreateSemaphores(VkDevice device, U32 count);

VkFence CreateFence(VkDevice device, VkFenceCreateFlags flags);

void CreateFences(VkDevice device, U32 count, VkFenceCreateFlags flags, std::vector<VkFence>& fences);

std::vector<VkFence> CreateFences(VkDevice device, U32 count, VkFenceCreateFlags flags);

}
} // namespace Vulkan
} // namespace Azura
