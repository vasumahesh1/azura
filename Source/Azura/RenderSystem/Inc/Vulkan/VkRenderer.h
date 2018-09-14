#pragma once

#include <vulkan/vulkan_core.h>

#include "Containers/Vector.h"
#include "Generic/Renderer.h"
#include "VkCore.h"
#include "VkPlatform.h"
#include "VkScopedBuffer.h"
#include "VkScopedPipeline.h"
#include "VkShader.h"
#include "Log/Log.h"

namespace Azura {
class Window;
}

namespace Azura {
namespace Vulkan {

class VkDrawablePool;

class VkDrawable final : public Drawable {
 public:
  VkDrawable(Memory::Allocator& allocator, VkDrawablePool& parentPool, Log logger);

  void AddVertexData(const Slot& slot, const U8* buffer, U32 size) override;
  void AddInstanceData(const Slot& slot, const U8* buffer, U32 size) override;
  void AddUniformData(const U8* buffer, U32 size, U32 binding) override;

  void SetIndexData(const U8* buffer, U32 size) override;
  void Submit() override;

  const VkDescriptorSet& GetDescriptorSet() const;

  void CleanUp(VkDevice device) const;

 private:
  VkDrawablePool& m_parentPool;
  VkDescriptorSet m_descriptorSet;
  const Log log_VulkanRenderSystem;
};

class VkDrawablePool final : public DrawablePool {
  friend class VkDrawable;

 public:
  VkDrawablePool(const DrawablePoolCreateInfo& createInfo,
                 VkDevice device,
                 VkQueue graphicsQueue,
                 VkBufferUsageFlags usage,
                 VkMemoryPropertyFlags memoryProperties,
                 VkPipelineLayout pipelineLayout,
                 VkDescriptorSetLayout descriptorSetLayout,
                 VkCommandPool graphicsCommandPool,
                 VkRenderPass renderPass,
                 const ApplicationRequirements& appReq,
                 const ViewportDimensions& viewport,
                 const VkPhysicalDeviceMemoryProperties& phyDeviceMemoryProperties,
                 const VkPhysicalDeviceProperties& physicalDeviceProperties,
                 const VkScopedSwapChain& swapChain,
                 Memory::Allocator& allocator,
                 Memory::Allocator& allocatorTemporary,
                 Log logger);

  Drawable& CreateDrawable() override;
  void AppendBytes(const U8* buffer, U32 bufferSize) override;
  void AppendBytes(const Containers::Vector<U8>& buffer) override;

  void CreateDescriptorPool(const ApplicationRequirements& appReq);

  void Submit() override;
  void AddBufferBinding(Slot slot, const Containers::Vector<RawStorageFormat>& strides) override;

  void AddShader(const Shader& shader) override;

  void CleanUp() const;

  const VkCommandBuffer& GetCommandBuffer() const;

 private:
  VkScopedBuffer m_buffer;
  VkScopedBuffer m_stagingBuffer;
  VkDevice m_device;
  VkRenderPass m_renderPass;
  ViewportDimensions m_viewport;

  VkDescriptorPool m_descriptorPool{};
  VkDescriptorSetLayout m_descriptorSetLayout;

  VkScopedPipeline m_pipeline;
  VkPipelineLayout m_pipelineLayout;
  VkPipelineFactory m_pipelineFactory;

  VkCommandBuffer m_commandBuffer{};
  VkCommandPool m_graphicsCommandPool;

  VkQueue m_graphicsQueue;

  const VkScopedSwapChain& m_swapChain;
  const ApplicationRequirements& m_appRequirements;
  const VkPhysicalDeviceProperties& m_physicalDeviceProperties;

  Containers::Vector<VkDrawable> m_drawables;
  Containers::Vector<VkShader> m_shaders;

  const Log log_VulkanRenderSystem;
};

class VkRenderer : public Renderer {
 public:
  VkRenderer(const ApplicationInfo& appInfo,
             const DeviceRequirements& deviceRequirements,
             const ApplicationRequirements& appRequirements,
             const SwapChainRequirements& swapChainRequirement,
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
  void Submit() override;
  void RenderFrame() override;

  void SnapshotFrame(const String& exportPath) const override;

 private:
  Window& m_window;
  Containers::Vector<VkDrawablePool> m_drawablePools;

#ifdef BUILD_DEBUG
  VkDebugReportCallbackEXT m_callback;
#endif

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
  Containers::Vector<VkFence> m_inFlightFences;
  Containers::Vector<VkCommandBuffer> m_primaryCommandBuffers;

  VkCommandPool m_graphicsCommandPool;
  VkCommandPool m_transferCommandPool;

  VkQueue m_graphicsQueue;
  VkQueue m_presentQueue;
  VkQueue m_transferQueue;

  VkPhysicalDeviceProperties m_physicalDeviceProperties{};

  std::reference_wrapper<Memory::Allocator> m_mainAllocator;
  std::reference_wrapper<Memory::Allocator> m_drawPoolAllocator;

  Log log_VulkanRenderSystem;
};
}  // namespace Vulkan
}  // namespace Azura
