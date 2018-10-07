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
#include "VkDrawablePool.h"
#include "VkTextureManager.h"

namespace Azura {
class Window;
}

namespace Azura {
namespace Vulkan {

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
