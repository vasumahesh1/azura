#include "Vulkan/VkRenderer.h"
#include "Utils/Macros.h"
#include "Memory/MonotonicAllocator.h"
#include "Memory/MemoryFactory.h"

using namespace Azura::Containers; // NOLINT - Freedom to use using namespace in CPP files.

namespace Azura {
namespace Vulkan {

VkDrawable::VkDrawable(U32 vertexCount, U32 indexCount, Memory::Allocator& allocator)
  : Drawable(vertexCount, indexCount, allocator),
    m_offset(allocator) {
}

VkDrawablePool::VkDrawablePool(U32 byteSize,
                               VkDevice device,
                               VkBufferUsageFlags usage,
                               VkMemoryPropertyFlags memoryProperties,
                               const VkPhysicalDeviceMemoryProperties& phyDeviceMemoryProperties,
                               Memory::Allocator& allocator)
  : DrawablePool(byteSize),
    m_buffer(device, usage, byteSize, memoryProperties, phyDeviceMemoryProperties),
    m_drawables(allocator) {
}

U32 VkDrawablePool::CreateDrawable(const DrawableCreateInfo& createInfo) {
  // TODO(vasumahesh1): Implement
  UNUSED(createInfo);
  return 0u;
}

Drawable& VkDrawablePool::GetDrawable(U32 id) {
  return m_drawables[id];
}

VkRenderer::VkRenderer(const ApplicationInfo& appInfo,
                       const DeviceRequirements& deviceRequirements,
                       const SwapChainRequirement& swapChainRequirement,
                       Memory::Allocator& mainAllocator,
                       Memory::Allocator& drawAllocator,
                       VkWindow& window)
  : Renderer(appInfo, deviceRequirements, mainAllocator),
    m_window(window),
    m_drawablePools(drawAllocator),
    m_swapChain(mainAllocator),
    m_frameBuffers(mainAllocator),
    m_imageAvailableSemaphores(mainAllocator),
    m_renderFinishedSemaphores(mainAllocator),
    mInFlightFences(mainAllocator) {

  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 2048);

  Vector<const char*> extensions(4, allocatorTemporary);
  VkWindow::GetInstanceExtensions(extensions);

  m_instance = VkCore::CreateInstance(GetApplicationInfo(), extensions);
  m_surface  = m_window.CreateSurface(m_instance);

  m_physicalDevice = VkCore::SelectPhysicalDevice(m_instance, m_surface, GetDeviceRequirements());
  m_queueIndices   = VkCore::FindQueueFamiliesInDevice(m_physicalDevice, m_surface, GetDeviceRequirements());
  m_device         = VkCore::CreateLogicalDevice(m_physicalDevice, m_queueIndices, GetDeviceRequirements());

  m_graphicsQueue = VkCore::GetQueueFromDevice(m_device, m_queueIndices.m_graphicsFamily);
  m_presentQueue  = VkCore::GetQueueFromDevice(m_device, m_queueIndices.m_presentFamily);

  const SwapChainDeviceSupport swapChainDeviceSupport = VkCore::QuerySwapChainSupport(m_physicalDevice, m_surface,
                                                                                      allocatorTemporary);

  if (m_queueIndices.m_isTransferQueueRequired) {
    m_transferQueue = VkCore::GetQueueFromDevice(m_device, m_queueIndices.m_transferFamily);
  }

  m_swapChain = VkCore::CreateSwapChain(m_device, m_surface, m_queueIndices, swapChainDeviceSupport,
                                        swapChainRequirement, mainAllocator);

  // TODO(vasumahesh1): Needs Changes

  m_renderPass = VkCore::CreateRenderPass(m_device, m_swapChain.m_surfaceFormat.format);

  Containers::Vector<VkDescriptorSetLayoutBinding> layoutBindings(allocatorTemporary);

  // TODO(vasumahesh1): Needs to be generic
  VkCore::CreateUniformBufferBinding(layoutBindings, VK_SHADER_STAGE_VERTEX_BIT);

  m_descriptorSetLayout = VkCore::CreateDescriptorSetLayout(m_device, layoutBindings);

  // TODO(vasumahesh1): Use Move or prevent this...
  Containers::Vector<VkDescriptorSetLayout> sets(allocatorTemporary);
  sets.PushBack(m_descriptorSetLayout);

  m_pipelineLayout = VkCore::CreatePipelineLayout(m_device, sets);
  VkCore::CreateFrameBuffers(m_device, m_renderPass, m_swapChain, m_frameBuffers);

  m_graphicsCommandPool = VkCore::CreateCommandPool(m_device, m_queueIndices.m_graphicsFamily, 0);

  if (m_queueIndices.m_isTransferQueueRequired) {
    m_transferCommandPool = VkCore::CreateCommandPool(m_device, m_queueIndices.m_transferFamily,
                                                      VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
  }

  const U32 syncCount = swapChainRequirement.m_framesInFlight;

  m_imageAvailableSemaphores.Resize(syncCount);
  m_renderFinishedSemaphores.Resize(syncCount);
  mInFlightFences.Resize(syncCount);

  VkCore::CreateSemaphores(m_device, syncCount, m_imageAvailableSemaphores);
  VkCore::CreateSemaphores(m_device, syncCount, m_renderFinishedSemaphores);
  VkCore::CreateFences(m_device, syncCount, VK_FENCE_CREATE_SIGNALED_BIT, mInFlightFences);
}

VkRenderer::~VkRenderer() = default;

U32 VkRenderer::CreateDrawablePool(const DrawablePoolCreateInfo& createInfo) {
  // TODO(vasumahesh1): Implement
  UNUSED(createInfo);
  return 0u;
}

DrawablePool& VkRenderer::GetDrawablePool(U32 id) {
  return m_drawablePools[id];
}

} // namespace Vulkan
} // namespace Azura
