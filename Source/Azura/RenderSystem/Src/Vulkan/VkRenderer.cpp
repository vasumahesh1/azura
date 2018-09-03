#include "Vulkan/VkRenderer.h"
#include "Generic/Window.h"
#include "Memory/MemoryFactory.h"
#include "Memory/MonotonicAllocator.h"
#include "Utils/Macros.h"
#include "Vulkan/VkShader.h"

using namespace Azura::Containers;  // NOLINT - Freedom to use using namespace in CPP files.

namespace Azura {
namespace Vulkan {

VkDrawable::VkDrawable(Memory::Allocator& allocator, VkDrawablePool& parentPool)
    : Drawable(allocator), m_parentPool(parentPool) {}

void VkDrawable::AddVertexData(const U8* buffer, U32 size, Slot slot) {

  BufferInfo info = BufferInfo();

  info.m_byteSize = size;
  info.m_offset   = m_parentPool.GetOffset();
  info.m_slot     = slot;

  m_vertexBufferInfos.PushBack(std::move(info));

  m_parentPool.AppendBytes(buffer, size);
}

void VkDrawable::AddInstanceData(const U8* buffer, U32 size, Slot slot) {
  BufferInfo info = BufferInfo();

  info.m_byteSize = size;
  info.m_offset   = m_parentPool.GetOffset();
  info.m_slot     = slot;

  m_instanceBufferInfos.PushBack(std::move(info));

  m_parentPool.AppendBytes(buffer, size);
}

void VkDrawable::SetIndexData(const U8* buffer, U32 size) {
  m_indexBufferInfo = BufferInfo();

  m_indexBufferInfo.m_byteSize = size;
  m_indexBufferInfo.m_offset   = m_parentPool.GetOffset();

  m_parentPool.AppendBytes(buffer, size);
}

VkDrawablePool::VkDrawablePool(U32 numDrawables,
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
                               Memory::Allocator& allocatorTemporary)
    : DrawablePool(byteSize, allocator),
      m_buffer(device, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, byteSize, memoryProperties, phyDeviceMemoryProperties),
      m_stagingBuffer(device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, byteSize, memoryProperties, phyDeviceMemoryProperties),
      m_device(device),
      m_renderPass(renderPass),
      m_viewport(viewport),
      m_pipeline({}),
      m_pipelineLayout(pipelineLayout),
      m_pipelineFactory(device, allocatorTemporary),
      m_swapChain(swapChain),
      m_drawables(numDrawables, allocator) {}

Drawable& VkDrawablePool::CreateDrawable() {
  VkDrawable drawable = VkDrawable(GetAllocator(), *this);
  m_drawables.PushBack(std::move(drawable));

  return m_drawables[m_drawables.GetSize() - 1];
}

void VkDrawablePool::AppendBytes(const U8* buffer, U32 bufferSize) {
  void* data = m_stagingBuffer.MapMemory(bufferSize, GetOffset());
  std::memcpy(data, buffer, bufferSize);
  m_stagingBuffer.UnMapMemory();

  // Record Offset Changes
  MoveOffset(bufferSize);
}

// TODO(vasumahesh1): Check behaviour
void VkDrawablePool::AppendBytes(const Containers::Vector<U8>& buffer) {
  DrawablePool::AppendBytes(buffer);
}

void VkDrawablePool::Submit() {
  m_pipelineFactory.SetInputAssemblyStage(PrimitiveTopology::TriangleList);
  m_pipelineFactory.SetRasterizerStage(CullMode::BackBit, FrontFace::CounterClockwise);
  m_pipelineFactory.SetPipelineLayout(m_pipelineLayout);
  m_pipelineFactory.SetRenderPass(m_renderPass);
  m_pipelineFactory.SetViewportStage(m_viewport, m_swapChain);
  m_pipelineFactory.SetMultisampleStage();
  m_pipelineFactory.SetColorBlendStage();

  m_pipeline = m_pipelineFactory.Submit();
}

void VkDrawablePool::AddBufferBinding(Slot slot, const Containers::Vector<RawStorageFormat>& strides) {
  m_pipelineFactory.BulkAddAttributeDescription(strides, slot.m_binding);

  U32 totalBufferStride = 0;

  for (const auto& stride : strides) {
    totalBufferStride += GetFormatSize(stride);
  }

  totalBufferStride /= 8;

  m_pipelineFactory.AddBindingDescription(totalBufferStride, slot);
}

void VkDrawablePool::AddShader(const Shader& shader) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
  auto vkShader = static_cast<const VkShader&>(shader);
  m_pipelineFactory.AddShaderStage(vkShader.GetShaderStageInfo());
}

VkRenderer::VkRenderer(const ApplicationInfo& appInfo,
                       const DeviceRequirements& deviceRequirements,
                       const SwapChainRequirement& swapChainRequirement,
                       Memory::Allocator& mainAllocator,
                       Memory::Allocator& drawAllocator,
                       Window& window)
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
  VkPlatform::GetInstanceExtensions(extensions);

  m_instance = VkCore::CreateInstance(GetApplicationInfo(), extensions);
  m_surface  = VkPlatform::CreateSurface(m_window.GetHandle(), m_instance);

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
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 2048);

  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

  // TODO(vasumahesh1): This isn't as performance optimized as it should be. We can probably find a way to insert a
  // buffer inside each pool?
  // Also, using default Viewport.
  VkDrawablePool pool = VkDrawablePool(createInfo.m_numDrawables, createInfo.m_byteSize, m_device,
                                       VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                       m_pipelineLayout, m_renderPass, m_window.GetViewport(), memProperties,
                                       m_swapChain, m_drawPoolAllocator, allocatorTemporary);

  m_drawablePools.PushBack(std::move(pool));

  return m_drawablePools[m_drawablePools.GetSize() - 1];
}

void VkRenderer::SetDrawablePoolCount(U32 count) {
  m_drawablePools.Reserve(count);
}

VkDevice VkRenderer::GetDevice() const {
  return m_device;
}

String VkRenderer::GetRenderingAPI() const {
  return "Vulkan";
}

}  // namespace Vulkan
}  // namespace Azura
