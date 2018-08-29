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

class VkDrawablePool;

class VkDrawable final : public Drawable {
public:
  VkDrawable(Memory::Allocator& allocator, VkDrawablePool& parentPool);

  void AddVertexData(const Containers::Vector<U8>& buffer, Containers::Vector<RawStorageFormat> strides) override;
  void AddInstanceData(const Containers::Vector<U8>& buffer, Containers::Vector<RawStorageFormat> strides) override;
  void SetIndexData(const Containers::Vector<U8>& buffer, RawStorageFormat stride) override;

private:
  VkDrawablePool& m_parentPool;
};

class VkDrawablePool final : public DrawablePool {
  friend class VkDrawable;

public:
  VkDrawablePool(U32 numDrawables,
                 U32 byteSize,
                 VkDevice device,
                 VkBufferUsageFlags usage,
                 VkMemoryPropertyFlags memoryProperties,
                 const VkPhysicalDeviceMemoryProperties& phyDeviceMemoryProperties,
                 Memory::Allocator& allocator);

  Drawable& CreateDrawable() override;
  void AppendBytes(const Containers::Vector<U8>& buffer) override;

private:
  VkScopedBuffer m_buffer;
  VkScopedBuffer m_stagingBuffer;
  VkDevice m_device;

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

  DrawablePool& CreateDrawablePool(const DrawablePoolCreateInfo& createInfo) override;

  void SetDrawablePoolCount(U32 count) override;
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

  std::reference_wrapper<Memory::Allocator> m_mainAllocator;
  std::reference_wrapper<Memory::Allocator> m_drawPoolAllocator;
};
} // namespace Vulkan
} // namespace Azura

