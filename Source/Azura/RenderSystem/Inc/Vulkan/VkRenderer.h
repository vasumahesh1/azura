#pragma once

#include <vulkan/vulkan_core.h>

#include "Containers/Vector.h"
#include "Generic/Renderer.h"
#include "VkCore.h"
#include "VkPlatform.h"
#include "VkScopedBuffer.h"
#include "VkScopedPipeline.h"

namespace Azura {
class Window;
}

namespace Azura {
namespace Vulkan {

class VkDrawablePool;

class VkDrawable final : public Drawable {
 public:
  VkDrawable(Memory::Allocator& allocator, VkDrawablePool& parentPool);

  void AddVertexData(const U8* buffer, U32 size, Slot slot) override;
  void AddInstanceData(const U8* buffer, U32 size, Slot slot) override;
  void SetIndexData(const U8* buffer, U32 size) override;

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
                 VkPipelineLayout pipelineLayout,
                 VkRenderPass renderPass,
                 const ViewportDimensions& viewport,
                 const VkPhysicalDeviceMemoryProperties& phyDeviceMemoryProperties,
                 const VkScopedSwapChain& swapChain,
                 Memory::Allocator& allocator,
                 Memory::Allocator& allocatorTemporary);

  Drawable& CreateDrawable() override;
  void AppendBytes(const U8* buffer, U32 bufferSize) override;
  void AppendBytes(const Containers::Vector<U8>& buffer) override;

  void Submit() override;
  void AddBufferBinding(Slot slot, const Containers::Vector<RawStorageFormat>& strides) override;

  void AddShader(const Shader& shader) override;

 private:
  VkScopedBuffer m_buffer;
  VkScopedBuffer m_stagingBuffer;
  VkDevice m_device;
  VkRenderPass m_renderPass;
  ViewportDimensions m_viewport;

  VkScopedPipeline m_pipeline;
  VkPipelineLayout m_pipelineLayout;
  VkPipelineFactory m_pipelineFactory;

  const VkScopedSwapChain& m_swapChain;

  Containers::Vector<VkDrawable> m_drawables;
};

class VkRenderer : public Renderer {
 public:
  VkRenderer(const ApplicationInfo& appInfo,
             const DeviceRequirements& deviceRequirements,
             const ApplicationRequirements& appRequirements,
             const SwapChainRequirement& swapChainRequirement,
             Memory::Allocator& mainAllocator,
             Memory::Allocator& drawAllocator,
             Window& window);
  ~VkRenderer();

  VkRenderer(const VkRenderer& other)     = delete;
  VkRenderer(VkRenderer&& other) noexcept = delete;
  VkRenderer& operator=(const VkRenderer& other) = delete;
  VkRenderer& operator=(VkRenderer&& other) noexcept = delete;

  DrawablePool& CreateDrawablePool(const DrawablePoolCreateInfo& createInfo) override;
  void SetDrawablePoolCount(U32 count) override;

  VkDevice GetDevice() const;
  String GetRenderingAPI() const override;

 private:
  Window& m_window;
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
}  // namespace Vulkan
}  // namespace Azura
