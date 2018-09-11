#pragma once

#include <vulkan/vulkan_core.h>

#include "Containers/Vector.h"
#include "Generic/Renderer.h"
#include "Types.h"
#include "VkTypes.h"

namespace Azura {
namespace Vulkan {
class VkScopedBuffer;
}
}

namespace Azura {
namespace Vulkan {
namespace VkCore {

#ifdef BUILD_DEBUG
VkDebugReportCallbackEXT SetupDebug(VkInstance instance);
void DestroyDebugReportCallbackEXT(VkInstance instance,
                                   VkDebugReportCallbackEXT callback,
                                   const VkAllocationCallbacks* pAllocator);
VkResult CreateDebugReportCallbackEXT(VkInstance instance,
                                      const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
                                      const VkAllocationCallbacks* pAllocator,
                                      VkDebugReportCallbackEXT* pCallback);
#endif

VkInstance CreateInstance(const ApplicationInfo& applicationData, const Containers::Vector<const char*>& vkExtensions);

SwapChainDeviceSupport QuerySwapChainSupport(VkPhysicalDevice device,
                                             VkSurfaceKHR surface,
                                             Memory::Allocator& allocator);

VkPhysicalDevice SelectPhysicalDevice(VkInstance instance,
                                      VkSurfaceKHR surface,
                                      const DeviceRequirements& requirements);
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
                                  const SwapChainRequirements& swapChainRequirement,
                                  Memory::Allocator& allocator);

VkImageView CreateImageView(VkDevice device,
                            VkImage sourceImage,
                            VkImageViewType viewType,
                            VkFormat viewFormat,
                            VkImageAspectFlags aspectMask,
                            U32 baseMip        = 0,
                            U32 levelCount     = 1,
                            U32 baseArrayLayer = 0,
                            U32 layerCount     = 1);

VkRenderPass CreateRenderPass(VkDevice device, VkFormat colorFormat);

void CreateUniformBufferBinding(Containers::Vector<VkDescriptorSetLayoutBinding>& bindings,
                                const UniformBufferDesc& desc,
                                VkShaderStageFlags stageFlag);
VkDescriptorSetLayout CreateDescriptorSetLayout(VkDevice device,
                                                const Containers::Vector<VkDescriptorSetLayoutBinding>& bindings);
VkPipelineLayout CreatePipelineLayout(VkDevice device, const Containers::Vector<VkDescriptorSetLayout>& descriptorSets);

VkShaderModule CreateShaderModule(VkDevice device, const Containers::Vector<U8>& code);

Containers::Vector<VkFramebuffer> CreateFrameBuffers(VkDevice device,
                                                     VkRenderPass renderPass,
                                                     const VkScopedSwapChain& scopedSwapChain,
                                                     Memory::Allocator& allocator);

void CreateFrameBuffers(VkDevice device,
                        VkRenderPass renderPass,
                        const VkScopedSwapChain& scopedSwapChain,
                        Containers::Vector<VkFramebuffer>& frameBuffers);

VkCommandPool CreateCommandPool(VkDevice device, int queueIndex, VkCommandPoolCreateFlags flags);

U32 FindMemoryType(U32 typeFilter,
                   VkMemoryPropertyFlags properties,
                   const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties);

VkDescriptorPoolSize CreateDescriptorPoolSize(VkDescriptorType type, U32 descriptorCount);

VkDescriptorPool CreateDescriptorPool(VkDevice device,
                                      const Containers::Vector<VkDescriptorPoolSize>& pools,
                                      U32 maxSets);

VkDescriptorSet CreateDescriptorSet(VkDevice device,
                                    VkDescriptorPool descriptorPool,
                                    const Containers::Vector<VkDescriptorSetLayout>& descriptorSets);

VkWriteDescriptorSet CreateWriteDescriptorForUniformBuffer(
  VkDescriptorSet set,
  U32 layoutIndex,
  U32 binding,
  const Containers::Vector<VkDescriptorBufferInfo>& bufferInfos);

void UpdateDescriptorSets(VkDevice device, const Containers::Vector<VkWriteDescriptorSet>& descriptorWrites);

VkCommandBuffer CreateCommandBuffer(VkDevice device, VkCommandPool commandPool, VkCommandBufferLevel level);

Containers::Vector<VkCommandBuffer> CreateCommandBuffers(
  VkDevice device,
  U32 count,
  VkCommandPool commandPool,
  VkCommandBufferLevel level,
  Memory::Allocator& allocator);

void CreateCommandBuffers(VkDevice device,
                          VkCommandPool commandPool,
                          VkCommandBufferLevel level,
                          Containers::Vector<VkCommandBuffer>& commandBuffers);

void BeginCommandBuffer(VkCommandBuffer buffer, VkCommandBufferUsageFlags flags);
void BeginCommandBuffer(VkCommandBuffer buffer,
                        VkCommandBufferUsageFlags flags,
                        const VkCommandBufferInheritanceInfo& inheritanceInfo);

void EndCommandBuffer(VkCommandBuffer buffer);

VkSemaphore CreateSemaphore(VkDevice device);

void CreateSemaphores(VkDevice device, U32 count, Containers::Vector<VkSemaphore>& semaphores);

Containers::Vector<VkSemaphore> CreateSemaphores(VkDevice device, U32 count, Memory::Allocator& allocator);

VkFence CreateFence(VkDevice device, VkFenceCreateFlags flags);

void CreateFences(VkDevice device, U32 count, VkFenceCreateFlags flags, Containers::Vector<VkFence>& fences);

Containers::Vector<VkFence> CreateFences(VkDevice device,
                                         U32 count,
                                         VkFenceCreateFlags flags,
                                         Memory::Allocator& allocator);

void CopyBuffer(VkDevice device,
                VkQueue queue,
                const VkScopedBuffer& srcBuffer,
                const VkScopedBuffer& dstBuffer,
                const VkDeviceSize size,
                const VkCommandPool commandPool);

} // namespace VkCore
} // namespace Vulkan
} // namespace Azura
