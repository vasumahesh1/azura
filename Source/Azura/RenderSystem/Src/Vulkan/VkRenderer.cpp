#include "Vulkan/VkRenderer.h"
#include "Memory/MemoryFactory.h"
#include "Memory/MonotonicAllocator.h"
#include "Utils/Macros.h"

using namespace Azura::Containers; // NOLINT - Freedom to use using namespace in CPP files.

namespace Azura {
namespace Vulkan {

VkDrawable::VkDrawable(Memory::Allocator& allocator, VkDrawablePool& parentPool)
  : Drawable(allocator),
    m_parentPool(parentPool) {
}

void VkDrawable::AddVertexData(const Containers::Vector<U8>& buffer, Containers::Vector<RawStorageFormat> strides) {
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 512);

  BufferInfo info = BufferInfo(allocatorTemporary);

  info.m_strideInfo = strides;

  info.m_byteSize = buffer.GetSize();
  info.m_offset   = m_parentPool.GetOffset();

  m_vertexBufferInfos.PushBack(std::move(info));

  m_parentPool.AppendBytes(buffer);
}

void VkDrawable::AddInstanceData(const Containers::Vector<U8>& buffer, Containers::Vector<RawStorageFormat> strides) {
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 512);

  BufferInfo info = BufferInfo(allocatorTemporary);

  info.m_strideInfo = strides;

  info.m_byteSize = buffer.GetSize();
  info.m_offset   = m_parentPool.GetOffset();

  m_instanceBufferInfos.PushBack(std::move(info));

  m_parentPool.AppendBytes(buffer);
}

void VkDrawable::SetIndexData(const Containers::Vector<U8>& buffer, RawStorageFormat stride) {
  m_indexBufferInfo = BufferInfo(GetAllocator());

  m_indexBufferInfo.m_strideInfo.Resize(1);
  m_indexBufferInfo.m_strideInfo.PushBack(stride);

  m_indexBufferInfo.m_byteSize = buffer.GetSize();
  m_indexBufferInfo.m_offset   = m_parentPool.GetOffset();

  m_parentPool.AppendBytes(buffer);

  SetIndexCount(buffer.GetSize() / GetFormatSize(stride));
}

VkDrawablePool::VkDrawablePool(U32 numDrawables,
                               U32 byteSize,
                               VkDevice device,
                               VkBufferUsageFlags usage,
                               VkMemoryPropertyFlags memoryProperties,
                               const VkPhysicalDeviceMemoryProperties& phyDeviceMemoryProperties,
                               Memory::Allocator& allocator)
  : DrawablePool(byteSize, allocator),
    m_buffer(device, usage, byteSize, memoryProperties, phyDeviceMemoryProperties),
    m_drawables(numDrawables, allocator) {
}

Drawable& VkDrawablePool::CreateDrawable() {
  VkDrawable drawable = VkDrawable(GetAllocator(), *this);
  m_drawables.PushBack(std::move(drawable));

  return m_drawables[m_drawables.GetSize() - 1];
}

void VkDrawablePool::AppendBytes(const Containers::Vector<U8>& buffer) { UNUSED(buffer); }

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
    mInFlightFences(mainAllocator),
    m_mainAllocator(mainAllocator),
    m_drawPoolAllocator(drawAllocator) {
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

  const SwapChainDeviceSupport swapChainDeviceSupport =
    VkCore::QuerySwapChainSupport(m_physicalDevice, m_surface, allocatorTemporary);

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
    m_transferCommandPool =
      VkCore::CreateCommandPool(m_device, m_queueIndices.m_transferFamily, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
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

DrawablePool& VkRenderer::CreateDrawablePool(const DrawablePoolCreateInfo& createInfo) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

  // TODO(vasumahesh1): This isn't as performance optimized as it should be. We can probably find a way to insert a buffer inside each pool?
  VkDrawablePool pool = VkDrawablePool(createInfo.m_numDrawables, createInfo.m_byteSize, m_device,
                                       VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memProperties, m_drawPoolAllocator);

  m_drawablePools.PushBack(std::move(pool));

  return m_drawablePools[m_drawablePools.GetSize() - 1];
}

void VkRenderer::SetDrawablePoolCount(U32 count) { m_drawablePools.Reserve(count); }

} // namespace Vulkan
} // namespace Azura
