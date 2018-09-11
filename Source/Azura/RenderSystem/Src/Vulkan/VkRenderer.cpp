#include "Vulkan/VkRenderer.h"
#include "Generic/Window.h"
#include "Memory/MemoryFactory.h"
#include "Memory/MonotonicAllocator.h"
#include "Utils/Macros.h"
#include "Vulkan/VkShader.h"
#include "Vulkan/VkTypeMapping.h"
#include "Vulkan/VkMacros.h"

using namespace Azura::Containers; // NOLINT - Freedom to use using namespace in CPP files.

namespace Azura {
namespace Vulkan {

VkDrawable::VkDrawable(Memory::Allocator& allocator, VkDrawablePool& parentPool)
  : Drawable(allocator),
    m_parentPool(parentPool),
    m_descriptorSet() {
}

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

void VkDrawable::AddUniformData(const U8* buffer, U32 size, U32 binding) {
  const auto minAlignment = U32(m_parentPool.m_physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);

  if (minAlignment > 0) {
    size = (size + minAlignment - 1) & ~(minAlignment - 1);
  }

  const U32 currentOffset = m_parentPool.GetOffset();
  const U32 newOffset     = (currentOffset + minAlignment - 1) & ~(minAlignment - 1);
  m_parentPool.MoveOffset(newOffset - currentOffset);

  // TODO(vasumahesh1): Creating a slot here, when only binding data needed.
  BufferInfo info = BufferInfo();
  info.m_byteSize = size;
  info.m_offset   = newOffset;
  info.m_slot     = Slot{binding};

  m_uniformBufferInfos.PushBack(std::move(info));

  m_parentPool.AppendBytes(buffer, size);
}

void VkDrawable::SetIndexData(const U8* buffer, U32 size) {
  m_indexBufferInfo = BufferInfo();

  m_indexBufferInfo.m_byteSize = size;
  m_indexBufferInfo.m_offset   = m_parentPool.GetOffset();

  m_parentPool.AppendBytes(buffer, size);
}

void VkDrawable::Submit() {
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 2048);

  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType                       = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool              = m_parentPool.m_descriptorPool;
  allocInfo.descriptorSetCount          = 1;
  allocInfo.pSetLayouts                 = &m_parentPool.m_descriptorSetLayout;

  VERIFY_VK_OP(vkAllocateDescriptorSets(m_parentPool.m_device, &allocInfo, &m_descriptorSet),
    "Failed to Allocate Descriptor Set");

  Vector<VkWriteDescriptorSet> descriptorWrites(m_uniformBufferInfos.GetSize(), allocatorTemporary);

  // TODO(vasumahesh1):[DESCRIPTOR]: How to use Uniform Buffer Arrays?
  // TODO(vasumahesh1):[TEXTURE]: Add support for Textures

  for (const auto& ubInfo : m_uniformBufferInfos) {
    VkDescriptorBufferInfo uniformBufferInfo = {};
    uniformBufferInfo.buffer                 = m_parentPool.m_buffer.Real();
    uniformBufferInfo.offset                 = ubInfo.m_offset;
    uniformBufferInfo.range                  = ubInfo.m_byteSize;

    VkWriteDescriptorSet uniformDescriptorWrite = {};
    uniformDescriptorWrite.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    uniformDescriptorWrite.dstSet               = m_descriptorSet;
    uniformDescriptorWrite.dstBinding           = 0;
    uniformDescriptorWrite.dstArrayElement      = 0;
    uniformDescriptorWrite.descriptorType       = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformDescriptorWrite.descriptorCount      = 1;
    uniformDescriptorWrite.pBufferInfo          = &uniformBufferInfo;
    uniformDescriptorWrite.pImageInfo           = nullptr;
    uniformDescriptorWrite.pTexelBufferView     = nullptr;

    descriptorWrites.PushBack(std::move(uniformDescriptorWrite));
  }

  vkUpdateDescriptorSets(m_parentPool.m_device, descriptorWrites.GetSize(), descriptorWrites.Data(), 0, nullptr);
}

const VkDescriptorSet& VkDrawable::GetDescriptorSet() const {
  return m_descriptorSet;
}

void VkDrawable::CleanUp(VkDevice device) const {
  UNUSED(device);
}

VkDrawablePool::VkDrawablePool(const DrawablePoolCreateInfo& createInfo,
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
                               Memory::Allocator& allocatorTemporary)
  : DrawablePool(createInfo.m_byteSize, allocator),
    m_buffer(device, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, createInfo.m_byteSize, memoryProperties,
             phyDeviceMemoryProperties),
    m_stagingBuffer(device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, createInfo.m_byteSize, memoryProperties,
                    phyDeviceMemoryProperties),
    m_device(device),
    m_renderPass(renderPass),
    m_viewport(viewport),
    m_descriptorSetLayout(descriptorSetLayout),
    m_pipeline({}),
    m_pipelineLayout(pipelineLayout),
    m_pipelineFactory(device, allocatorTemporary),
    m_graphicsCommandPool(graphicsCommandPool),
  m_graphicsQueue(graphicsQueue),
    m_swapChain(swapChain),
    m_appRequirements(appReq),
    m_physicalDeviceProperties(physicalDeviceProperties),
    m_drawables(createInfo.m_numDrawables, allocator),
    m_shaders(createInfo.m_numShaders, allocator) {
}


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

void VkDrawablePool::CreateDescriptorPool(const ApplicationRequirements& appReq) {
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 2048);

  // TODO(vasumahesh1):[DESCRIPTOR]: How to use Uniform Buffer Arrays?
  VkDescriptorPoolSize uniformPoolSize = {};
  uniformPoolSize.type                 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
  uniformPoolSize.descriptorCount      = appReq.m_uniformBuffers.GetSize();

  // TODO(vasumahesh1):[TEXTURE]: Add support for Descriptor Pools
  Vector<VkDescriptorPoolSize> descriptorPool(1, allocatorTemporary);
  descriptorPool.PushBack(uniformPoolSize);

  // TODO(vasumahesh1):[DESCRIPTOR]: 1 Set per Drawable?
  VkDescriptorPoolCreateInfo poolInfo = {};
  poolInfo.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount              = descriptorPool.GetSize();
  poolInfo.pPoolSizes                 = descriptorPool.Data();
  poolInfo.maxSets                    = m_drawables.GetSize();

  VERIFY_VK_OP(vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool),
    "Unable to create Descriptor Pool");
}

void VkDrawablePool::Submit() {
  CreateDescriptorPool(m_appRequirements);

  m_pipelineFactory.SetInputAssemblyStage(PrimitiveTopology::TriangleList);
  m_pipelineFactory.SetRasterizerStage(CullMode::None, FrontFace::CounterClockwise);
  m_pipelineFactory.SetPipelineLayout(m_pipelineLayout);
  m_pipelineFactory.SetRenderPass(m_renderPass);
  m_pipelineFactory.SetViewportStage(m_viewport, m_swapChain);
  m_pipelineFactory.SetMultisampleStage();
  m_pipelineFactory.SetColorBlendStage();

  m_pipeline = m_pipelineFactory.Submit();

  m_commandBuffer = VkCore::CreateCommandBuffer(m_device, m_graphicsCommandPool, VK_COMMAND_BUFFER_LEVEL_SECONDARY);

  VkCommandBufferInheritanceInfo inheritanceInfo = {};
  inheritanceInfo.sType                          = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
  inheritanceInfo.renderPass                     = m_renderPass;
  inheritanceInfo.framebuffer                    = VK_NULL_HANDLE;

  VkCore::BeginCommandBuffer(m_commandBuffer, VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, inheritanceInfo);

  vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.Real());

  VkCore::CopyBuffer(m_device, m_graphicsQueue, m_stagingBuffer, m_buffer, GetOffset(), m_graphicsCommandPool);

  VkBuffer mainBuffer = m_buffer.Real();

  for (auto& drawable : m_drawables) {
    drawable.Submit();

    const auto& vertBufferInfos = drawable.GetVertexBufferInfos();

    for (const auto& vertexBuffer : vertBufferInfos) {
      VkDeviceSize offsets[] = {vertexBuffer.m_offset};
      vkCmdBindVertexBuffers(m_commandBuffer, vertexBuffer.m_slot.m_binding, 1, &mainBuffer, &offsets[0]);
    }

    const auto& instanceBufferInfos = drawable.GetInstanceBufferInfos();

    for (const auto& instanceBuffer : instanceBufferInfos) {
      VkDeviceSize offsets[] = {instanceBuffer.m_offset};
      vkCmdBindVertexBuffers(m_commandBuffer, instanceBuffer.m_slot.m_binding, 1, &mainBuffer, &offsets[0]);
    }

    const auto& indexBufferInfo = drawable.GetIndexBufferInfo();

    const auto indexType = ToVkIndexType(drawable.GetIndexType());
    VERIFY_OPT(indexType, "Invalid VkIndexType converted");

    vkCmdBindIndexBuffer(m_commandBuffer, mainBuffer, indexBufferInfo.m_offset, indexType.value());

    vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1,
                            &drawable.GetDescriptorSet(), 0, nullptr);

    switch (drawable.GetDrawType()) {
      case DrawType::InstancedIndexed:
        vkCmdDrawIndexed(m_commandBuffer, drawable.GetIndexCount(), drawable.GetInstanceCount(), 0, 0, 0);
        break;

      case DrawType::InstancedIndexedIndirect:
        break;

      default:
        break;
    }
  }

  VkCore::EndCommandBuffer(m_commandBuffer);
}

void VkDrawablePool::AddBufferBinding(Slot slot, const Containers::Vector<RawStorageFormat>& strides) {
  m_pipelineFactory.BulkAddAttributeDescription(strides, slot.m_binding);

  U32 totalBufferStride = 0;

  for (const auto& stride : strides) {
    totalBufferStride += GetFormatSize(stride);
  }

  m_pipelineFactory.AddBindingDescription(totalBufferStride, slot);
}

void VkDrawablePool::AddShader(const Shader& shader) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
  auto vkShader = static_cast<const VkShader&>(shader);

  m_shaders.PushBack(vkShader);
  m_pipelineFactory.AddShaderStage(m_shaders[m_shaders.GetSize() - 1].GetShaderStageInfo());
}

void VkDrawablePool::CleanUp() const {
  m_buffer.CleanUp();
  m_stagingBuffer.CleanUp();

  m_pipeline.CleanUp(m_device);

  vkFreeCommandBuffers(m_device, m_graphicsCommandPool, 1, &m_commandBuffer);

  for (const auto& shader : m_shaders) {
    shader.CleanUp(m_device);
  }
}

const VkCommandBuffer& VkDrawablePool::GetCommandBuffer() const {
  return m_commandBuffer;
}

VkRenderer::VkRenderer(const ApplicationInfo& appInfo,
                       const DeviceRequirements& deviceRequirements,
                       const ApplicationRequirements& appRequirements,
                       const SwapChainRequirements& swapChainRequirement,
                       Memory::Allocator& mainAllocator,
                       Memory::Allocator& drawAllocator,
                       Window& window)
  : Renderer(appInfo, deviceRequirements, appRequirements, swapChainRequirement, mainAllocator),
    m_window(window),
    m_drawablePools(drawAllocator),
    m_swapChain(mainAllocator),
    m_frameBuffers(mainAllocator),
    m_imageAvailableSemaphores(mainAllocator),
    m_renderFinishedSemaphores(mainAllocator),
    m_inFlightFences(mainAllocator),
    m_primaryCommandBuffers(mainAllocator),
    m_mainAllocator(mainAllocator),
    m_drawPoolAllocator(drawAllocator) {
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 16384);

  Vector<const char*> extensions(4, allocatorTemporary);
  VkPlatform::GetInstanceExtensions(extensions);

  m_instance = VkCore::CreateInstance(GetApplicationInfo(), extensions);
#ifdef BUILD_DEBUG
  m_callback = VkCore::SetupDebug(m_instance);
#endif

  m_surface = VkPlatform::CreateSurface(m_window.GetHandle(), m_instance);

  m_physicalDevice = VkCore::SelectPhysicalDevice(m_instance, m_surface, GetDeviceRequirements());
  m_queueIndices   = VkCore::FindQueueFamiliesInDevice(m_physicalDevice, m_surface, GetDeviceRequirements());
  m_device         = VkCore::CreateLogicalDevice(m_physicalDevice, m_queueIndices, GetDeviceRequirements());

  vkGetPhysicalDeviceProperties(m_physicalDevice, &m_physicalDeviceProperties);

  m_graphicsQueue = VkCore::GetQueueFromDevice(m_device, m_queueIndices.m_graphicsFamily);
  m_presentQueue  = VkCore::GetQueueFromDevice(m_device, m_queueIndices.m_presentFamily);

  const SwapChainDeviceSupport swapChainDeviceSupport =
    VkCore::QuerySwapChainSupport(m_physicalDevice, m_surface, allocatorTemporary);

  if (m_queueIndices.m_isTransferQueueRequired) {
    m_transferQueue = VkCore::GetQueueFromDevice(m_device, m_queueIndices.m_transferFamily);
  }

  m_swapChain = VkCore::CreateSwapChain(m_device, m_surface, m_queueIndices, swapChainDeviceSupport,
                                        swapChainRequirement, mainAllocator);

  // TODO(vasumahesh1):[RENDER PASS]: Needs Changes
  m_renderPass = VkCore::CreateRenderPass(m_device, m_swapChain.m_surfaceFormat.format);

  const U32 uniformCount = appRequirements.m_uniformBuffers.GetSize();
  Containers::Vector<VkDescriptorSetLayoutBinding> layoutBindings(uniformCount, allocatorTemporary);

  for (const auto& bufferDesc : appRequirements.m_uniformBuffers) {
    const auto vkShaderStage = ToVkShaderStageFlagBits(bufferDesc.first);
    VERIFY_OPT(vkShaderStage, "Unknown Shader Stage");

    VkCore::CreateUniformBufferBinding(layoutBindings, bufferDesc.second, vkShaderStage.value());
  }

  m_descriptorSetLayout = VkCore::CreateDescriptorSetLayout(m_device, layoutBindings);

  // TODO(vasumahesh1):[DESCRIPTOR]: Add support for more than 1 set
  const Containers::Vector<VkDescriptorSetLayout> sets({m_descriptorSetLayout}, allocatorTemporary);
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
  m_inFlightFences.Resize(syncCount);

  VkCore::CreateSemaphores(m_device, syncCount, m_imageAvailableSemaphores);
  VkCore::CreateSemaphores(m_device, syncCount, m_renderFinishedSemaphores);
  VkCore::CreateFences(m_device, syncCount, VK_FENCE_CREATE_SIGNALED_BIT, m_inFlightFences);
}

VkRenderer::~VkRenderer() {
  vkDeviceWaitIdle(m_device);

#ifdef BUILD_DEBUG
  VkCore::DestroyDebugReportCallbackEXT(m_instance, m_callback, nullptr);
#endif

  for (const auto& buffer : m_frameBuffers) {
    vkDestroyFramebuffer(m_device, buffer, nullptr);
  }

  for (const auto& semaphore : m_imageAvailableSemaphores) {
    vkDestroySemaphore(m_device, semaphore, nullptr);
  }

  for (const auto& semaphore : m_renderFinishedSemaphores) {
    vkDestroySemaphore(m_device, semaphore, nullptr);
  }

  for (const auto& fences : m_inFlightFences) {
    vkDestroyFence(m_device, fences, nullptr);
  }

  for (const auto& pool : m_drawablePools) {
    pool.CleanUp();
  }

  m_swapChain.CleanUp(m_device);

  vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
  vkDestroyRenderPass(m_device, m_renderPass, nullptr);

  vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayout, nullptr);

  vkDestroyCommandPool(m_device, m_graphicsCommandPool, nullptr);
  vkDestroyCommandPool(m_device, m_transferCommandPool, nullptr);

  vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

  vkDestroyDevice(m_device, nullptr);     // Queues are also deleted
  vkDestroyInstance(m_instance, nullptr); // m_device also Deleted
};

DrawablePool& VkRenderer::CreateDrawablePool(const DrawablePoolCreateInfo& createInfo) {
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 2048);

  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

  // TODO(vasumahesh1): This isn't as performance optimized as it should be. We can probably find a way to insert a
  // buffer inside each pool?
  // Also, using default Viewport.
  VkDrawablePool pool = VkDrawablePool(createInfo, m_device, m_graphicsQueue,
                                       VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                       VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                       m_pipelineLayout, m_descriptorSetLayout, m_graphicsCommandPool, m_renderPass,
                                       GetApplicationRequirements(), m_window.GetViewport(), memProperties,
                                       m_physicalDeviceProperties,
                                       m_swapChain, m_drawPoolAllocator, m_mainAllocator);

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

void VkRenderer::Submit() {
  m_primaryCommandBuffers.Resize(m_frameBuffers.GetSize());
  VkCore::CreateCommandBuffers(m_device, m_graphicsCommandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                               m_primaryCommandBuffers);

  const auto& clearColor = GetApplicationRequirements().m_clearColor;

  const VkClearValue clearValue = {clearColor[0], clearColor[1], clearColor[2], clearColor[3]};

  for (U32 idx                = 0; idx < m_primaryCommandBuffers.GetSize(); ++idx) {
    const auto& commandBuffer = m_primaryCommandBuffers[idx];
    VkCore::BeginCommandBuffer(commandBuffer, VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType                 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass            = m_renderPass;
    renderPassInfo.framebuffer           = m_frameBuffers[idx];
    renderPassInfo.renderArea.offset     = {0, 0};
    renderPassInfo.renderArea.extent     = m_swapChain.m_extent;
    renderPassInfo.clearValueCount       = 1;
    renderPassInfo.pClearValues          = &clearValue;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

    for (auto& drawablePool : m_drawablePools) {
      drawablePool.Submit();
      vkCmdExecuteCommands(commandBuffer, 1, &drawablePool.GetCommandBuffer());
    }

    vkCmdEndRenderPass(commandBuffer);

    VkCore::EndCommandBuffer(commandBuffer);
  }
}

void VkRenderer::RenderFrame() {
  EnterRenderFrame();

  const auto& currentFrame = GetCurrentFrame();

  vkWaitForFences(m_device, 1, &m_inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
  vkResetFences(m_device, 1, &m_inFlightFences[currentFrame]);

  uint32_t imageIndex;
  const VkResult result = vkAcquireNextImageKHR(m_device, m_swapChain.Real(), std::numeric_limits<uint64_t>::max(),
                                                m_imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    // TODO(vasumahesh1):[RESIZE]: Recreate Swapchain
    // RecreateSwapChain();
    return;
  }

  VERIFY_TRUE(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "Failed to acquire swap chain image");

  // SUCCESS OR SUBOPTIMAL
  VkSubmitInfo submitInfo = {};
  submitInfo.sType        = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  std::array<VkSemaphore, 1> waitSemaphores = {m_imageAvailableSemaphores[currentFrame]};
  std::array<VkPipelineStageFlags, 1> waitStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
  submitInfo.waitSemaphoreCount     = 1;
  submitInfo.pWaitSemaphores        = waitSemaphores.data();
  submitInfo.pWaitDstStageMask      = waitStages.data();

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers    = &m_primaryCommandBuffers[imageIndex];

  std::array<VkSemaphore, 1> signalSemaphores = { m_renderFinishedSemaphores[currentFrame] };
  submitInfo.signalSemaphoreCount = U32(signalSemaphores.size());
  submitInfo.pSignalSemaphores    = signalSemaphores.data();

  VERIFY_VK_OP(vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFences[currentFrame]),
    "Failed to submit draw command buffer");

  VkPresentInfoKHR presentInfo   = {};
  presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores    = signalSemaphores.data();

  std::array<VkSwapchainKHR, 1> swapChains = { m_swapChain.Real() };
  presentInfo.swapchainCount        = U32(swapChains.size());
  presentInfo.pSwapchains           = swapChains.data();
  presentInfo.pImageIndices         = &imageIndex;
  presentInfo.pResults              = nullptr;

  vkQueuePresentKHR(m_presentQueue, &presentInfo);

  ExitRenderFrame();
}

} // namespace Vulkan
} // namespace Azura
