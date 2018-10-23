#pragma once

#include <vulkan/vulkan_core.h>

#include "Containers/Vector.h"
#include "Generic/Renderer.h"
#include "VkCore.h"
#include "Vulkan/VkPlatform.h"
#include "Vulkan/VkScopedBuffer.h"
#include "Vulkan/VkScopedPipeline.h"
#include "Vulkan/VkShader.h"
#include "Log/Log.h"
#include "Vulkan/VkDrawablePool.h"
#include "Vulkan/VkTextureManager.h"
#include "Vulkan/VkScopedSwapChain.h"
#include "Vulkan/VkScopedRenderPass.h"
#include "Vulkan/VkComputePool.h"

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
             const RenderPassRequirements& renderPassRequirements,
             const DescriptorRequirements& descriptorRequirements,
             const ShaderRequirements& shaderRequirements,
             Memory::Allocator& mainAllocator,
             Memory::Allocator& drawAllocator,
             Window& window);

  ~VkRenderer();

  VkRenderer(const VkRenderer& other) = delete;
  VkRenderer(VkRenderer&& other) noexcept = delete;
  VkRenderer& operator=(const VkRenderer& other) = delete;
  VkRenderer& operator=(VkRenderer&& other) noexcept = delete;

  DrawablePool& CreateDrawablePool(const DrawablePoolCreateInfo& createInfo) override;
  ComputePool& CreateComputePool(const ComputePoolCreateInfo& createInfo) override;

  VkDevice GetDevice() const;
  String GetRenderingAPI() const override;
  void Submit() override;
  void CreateDescriptorInfo();
  void RenderFrame() override;

  void SnapshotFrame(const String& exportPath) const override;

private:
  void AddShader(const ShaderCreateInfo& info) override;

  Log log_VulkanRenderSystem;

  Memory::HeapMemoryBuffer m_perFrameBuffer;
  Memory::MonotonicAllocator m_perFrameAllocator;

  Containers::Vector<VkDrawablePool> m_drawablePools;
  Containers::Vector<VkComputePool> m_computePools;

#ifdef BUILD_DEBUG
  VkDebugReportCallbackEXT m_callback;
#endif

  VkInstance m_instance;
  VkSurfaceKHR m_surface;
  VkPhysicalDevice m_physicalDevice;
  VkQueueIndices m_queueIndices;
  VkDevice m_device;
  VkScopedSwapChain m_swapChain;
  Containers::Vector<VkScopedRenderPass> m_renderPasses;
  Containers::Vector<VkDescriptorSetLayout> m_descriptorSetLayouts;

  Containers::Vector<VkSemaphore> m_imageAvailableSemaphores;
  Containers::Vector<VkSemaphore> m_renderFinishedSemaphores;
  Containers::Vector<VkFence> m_inFlightFences;

  Containers::Vector<VkShader> m_shaders;

  Containers::Vector<VkScopedImage> m_renderPassAttachmentImages;

  VkCommandPool m_graphicsCommandPool;
  VkCommandPool m_transferCommandPool;

  VkQueue m_graphicsQueue;
  VkQueue m_presentQueue;
  VkQueue m_transferQueue;

  VkPhysicalDeviceProperties m_physicalDeviceProperties{};

  VkPipelineLayout m_pipelineLayout{};
  VkDescriptorPool m_descriptorPool{};

};
} // namespace Vulkan
} // namespace Azura
