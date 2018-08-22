#pragma once

#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <array>
#include <vector>

#include "Containers/Vector.h"
#include "Generic/Renderer.h"
#include "VkCore.h"
#include "VkScopedBuffer.h"
#include "VkWindow.h"

namespace Azura {
namespace Vulkan {

class VkDrawable final : public Drawable {
public:
  VkDrawable(U32 vertexCount,
             U32 indexCount,
             Memory::Allocator& allocator);

private:
  Containers::Vector<BufferInfo> m_offset;
};

class VkDrawablePool final : public DrawablePool {

public:
  VkDrawablePool(U32 byteSize,
                 VkDevice device,
                 VkBufferUsageFlags usage,
                 VkMemoryPropertyFlags memoryProperties,
                 const VkPhysicalDeviceMemoryProperties& phyDeviceMemoryProperties,
                 Memory::Allocator& allocator);

  U32 CreateDrawable(const DrawableCreateInfo& createInfo) override;
  Azura::Drawable& GetDrawable(U32 id) override;

private:
  VkScopedBuffer m_buffer;
  Containers::Vector<VkDrawable> m_drawables;
};

class VkRenderer : public Renderer {
public:
  VkRenderer(const ApplicationInfo& appInfo,
             const DeviceRequirements& deviceRequirements,
             const SwapChainRequirement& swapChainRequirement,
             Memory::Allocator& mainAllocator,
    Memory::Allocator& drawAllocator,
             VkWindow& window);
  ~VkRenderer();

  VkRenderer(const VkRenderer& other) = delete;
  VkRenderer(VkRenderer&& other) noexcept = delete;
  VkRenderer& operator=(const VkRenderer& other) = delete;
  VkRenderer& operator=(VkRenderer&& other) noexcept = delete;

  U32 CreateDrawablePool(const DrawablePoolCreateInfo& createInfo) override;
  DrawablePool& GetDrawablePool(U32 id) override;

private:
  VkWindow& m_window;
  Containers::Vector<VkDrawablePool> m_drawablePools;

  VkInstance m_instance;
  VkSurfaceKHR m_surface;
  VkPhysicalDevice m_physicalDevice;
  VkQueueIndices m_queueIndices;
  VkDevice m_device;
  VkScopedSwapChain m_swapChain;
  VkRenderPass m_renderPass;
  VkDescriptorSetLayout m_descriptorSetLayout;
  VkPipelineLayout m_pipelineLayout;

  Containers::Vector<VkFramebuffer> m_frameBuffers;
  Containers::Vector<VkSemaphore> m_imageAvailableSemaphores;
  Containers::Vector<VkSemaphore> m_renderFinishedSemaphores;
  Containers::Vector<VkFence> mInFlightFences;

  VkCommandPool m_graphicsCommandPool;
  VkCommandPool m_transferCommandPool;

  VkQueue m_graphicsQueue;
  VkQueue m_presentQueue;
  VkQueue m_transferQueue;
};
} // namespace Vulkan
} // namespace Azura

