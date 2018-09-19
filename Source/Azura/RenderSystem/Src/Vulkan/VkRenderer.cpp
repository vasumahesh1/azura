#include "Vulkan/VkRenderer.h"
#include "Generic/Window.h"
#include "Memory/MemoryFactory.h"
#include "Memory/MonotonicAllocator.h"
#include "Utils/Macros.h"
#include "Vulkan/VkShader.h"
#include "Vulkan/VkTypeMapping.h"
#include "Vulkan/VkMacros.h"
#include <fstream>

using namespace Azura::Containers; // NOLINT - Freedom to use using namespace in CPP files.

namespace Azura {
namespace Vulkan {

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
    m_drawPoolAllocator(drawAllocator),
    log_VulkanRenderSystem(Log("VulkanRenderSystem")) {
  HEAP_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 16384);

  Vector<const char*> extensions(4, allocatorTemporary);
  VkPlatform::GetInstanceExtensions(extensions);

  m_instance = VkCore::CreateInstance(GetApplicationInfo(), extensions, log_VulkanRenderSystem);
#ifdef BUILD_DEBUG
  m_callback = VkCore::SetupDebug(m_instance, log_VulkanRenderSystem);
#endif

  m_surface = VkPlatform::CreateSurface(m_window.GetHandle(), m_instance, log_VulkanRenderSystem);

  m_physicalDevice = VkCore::SelectPhysicalDevice(m_instance, m_surface, GetDeviceRequirements(),
                                                  log_VulkanRenderSystem);
  m_queueIndices = VkCore::FindQueueFamiliesInDevice(m_physicalDevice, m_surface, GetDeviceRequirements(),
                                                     log_VulkanRenderSystem);
  m_device = VkCore::CreateLogicalDevice(m_physicalDevice, m_queueIndices, GetDeviceRequirements(),
                                         log_VulkanRenderSystem);

  vkGetPhysicalDeviceProperties(m_physicalDevice, &m_physicalDeviceProperties);

  m_graphicsQueue = VkCore::GetQueueFromDevice(m_device, m_queueIndices.m_graphicsFamily);
  m_presentQueue  = VkCore::GetQueueFromDevice(m_device, m_queueIndices.m_presentFamily);

  const SwapChainDeviceSupport swapChainDeviceSupport =
    VkCore::QuerySwapChainSupport(m_physicalDevice, m_surface, allocatorTemporary);

  if (m_queueIndices.m_isTransferQueueRequired) {
    m_transferQueue = VkCore::GetQueueFromDevice(m_device, m_queueIndices.m_transferFamily);
  }

  m_swapChain = VkCore::CreateSwapChain(m_device, m_surface, m_queueIndices, swapChainDeviceSupport,
                                        swapChainRequirement, mainAllocator, log_VulkanRenderSystem);

  // TODO(vasumahesh1):[RENDER PASS]: Needs Changes
  m_renderPass = VkCore::CreateRenderPass(m_device, m_swapChain.m_surfaceFormat.format, log_VulkanRenderSystem);

  VkCore::CreateFrameBuffers(m_device, m_renderPass, m_swapChain, m_frameBuffers, log_VulkanRenderSystem);

  m_graphicsCommandPool = VkCore::CreateCommandPool(m_device, m_queueIndices.m_graphicsFamily, 0,
                                                    log_VulkanRenderSystem);

  if (m_queueIndices.m_isTransferQueueRequired) {
    m_transferCommandPool =
      VkCore::CreateCommandPool(m_device, m_queueIndices.m_transferFamily, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
                                log_VulkanRenderSystem);
  }

  const U32 syncCount = swapChainRequirement.m_framesInFlight;

  m_imageAvailableSemaphores.Resize(syncCount);
  m_renderFinishedSemaphores.Resize(syncCount);
  m_inFlightFences.Resize(syncCount);

  VkCore::CreateSemaphores(m_device, syncCount, m_imageAvailableSemaphores, log_VulkanRenderSystem);
  VkCore::CreateSemaphores(m_device, syncCount, m_renderFinishedSemaphores, log_VulkanRenderSystem);
  VkCore::CreateFences(m_device, syncCount, VK_FENCE_CREATE_SIGNALED_BIT, m_inFlightFences, log_VulkanRenderSystem);
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

  vkFreeCommandBuffers(m_device, m_graphicsCommandPool, m_primaryCommandBuffers.GetSize(),
                       m_primaryCommandBuffers.Data());

  m_swapChain.CleanUp(m_device);

  vkDestroyRenderPass(m_device, m_renderPass, nullptr);

  vkDestroyCommandPool(m_device, m_graphicsCommandPool, nullptr);

  if (m_queueIndices.m_isTransferQueueRequired) {
    vkDestroyCommandPool(m_device, m_transferCommandPool, nullptr);
  }

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
                                       m_graphicsCommandPool, m_renderPass,
                                       GetApplicationRequirements(), m_window.GetViewport(), memProperties,
                                       m_physicalDeviceProperties,
                                       m_swapChain, m_drawPoolAllocator, m_mainAllocator, log_VulkanRenderSystem);

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
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 1024);
  m_primaryCommandBuffers.Resize(m_frameBuffers.GetSize());
  VkCore::CreateCommandBuffers(m_device, m_graphicsCommandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                               m_primaryCommandBuffers, log_VulkanRenderSystem);

  const auto& clearColor = GetApplicationRequirements().m_clearColor;

  const VkClearValue clearValue = {clearColor[0], clearColor[1], clearColor[2], clearColor[3]};

  Vector<VkCommandBuffer> secondaryBuffers(m_drawablePools.GetSize(), allocatorTemporary);
  for (auto& drawablePool : m_drawablePools) {
    drawablePool.Submit();
    secondaryBuffers.PushBack(drawablePool.GetCommandBuffer());
  }

  for (U32 idx                = 0; idx < m_primaryCommandBuffers.GetSize(); ++idx) {
    const auto& commandBuffer = m_primaryCommandBuffers[idx];
    VkCore::BeginCommandBuffer(commandBuffer, VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, log_VulkanRenderSystem);

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType                 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass            = m_renderPass;
    renderPassInfo.framebuffer           = m_frameBuffers[idx];
    renderPassInfo.renderArea.offset     = {0, 0};
    renderPassInfo.renderArea.extent     = m_swapChain.m_extent;
    renderPassInfo.clearValueCount       = 1;
    renderPassInfo.pClearValues          = &clearValue;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

    vkCmdExecuteCommands(commandBuffer, secondaryBuffers.GetSize(), secondaryBuffers.Data());

    vkCmdEndRenderPass(commandBuffer);

    VkCore::EndCommandBuffer(commandBuffer, log_VulkanRenderSystem);
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

  VERIFY_TRUE(log_VulkanRenderSystem, result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR,
    "Failed to acquire swap chain image");

  // SUCCESS OR SUBOPTIMAL
  VkSubmitInfo submitInfo = {};
  submitInfo.sType        = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  std::array<VkSemaphore, 1> waitSemaphores      = {m_imageAvailableSemaphores[currentFrame]};
  std::array<VkPipelineStageFlags, 1> waitStages = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount                  = 1;
  submitInfo.pWaitSemaphores                     = waitSemaphores.data();
  submitInfo.pWaitDstStageMask                   = waitStages.data();

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers    = &m_primaryCommandBuffers[imageIndex];

  std::array<VkSemaphore, 1> signalSemaphores = {m_renderFinishedSemaphores[currentFrame]};
  submitInfo.signalSemaphoreCount             = U32(signalSemaphores.size());
  submitInfo.pSignalSemaphores                = signalSemaphores.data();

  VERIFY_VK_OP(log_VulkanRenderSystem, vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFences[currentFrame]),
    "Failed to submit draw command buffer");

  VkPresentInfoKHR presentInfo   = {};
  presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores    = signalSemaphores.data();

  std::array<VkSwapchainKHR, 1> swapChains = {m_swapChain.Real()};
  presentInfo.swapchainCount               = U32(swapChains.size());
  presentInfo.pSwapchains                  = swapChains.data();
  presentInfo.pImageIndices                = &imageIndex;
  presentInfo.pResults                     = nullptr;

  vkQueuePresentKHR(m_presentQueue, &presentInfo);

  ExitRenderFrame();
}

void VkRenderer::SnapshotFrame(const String& exportPath) const {
  // TODO(vasumahesh):[TEXTURE]: VkScopedImage
  VkDeviceMemory dstMemory;

  // TODO(vasumahesh1):[SNAPSHOT]: Use same format as Swap Chain currently
  const RawStorageFormat storageFormat = GetSwapchainRequirements().m_format;

  const bool supportsBlit = [this, storageFormat]() -> bool
  {
    const auto vkFormat = ToVkFormat(storageFormat);
    VERIFY_OPT(log_VulkanRenderSystem, vkFormat, "Unknown Vk Format");

    VkFormatProperties formatProps;
    vkGetPhysicalDeviceFormatProperties(m_physicalDevice, m_swapChain.m_surfaceFormat.format, &formatProps);
    if ((formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT) == 0u) {
      LOG_WRN(log_VulkanRenderSystem, LOG_LEVEL, "Swapchain Format doesn't support Blit, Will use Image Copy");
      return false;
    }

    vkGetPhysicalDeviceFormatProperties(m_physicalDevice, vkFormat.value(), &formatProps);
    if ((formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT) == 0u) {
      LOG_WRN(log_VulkanRenderSystem, LOG_LEVEL,
        "Destination Image Format doesn't support linear blit, Will use Image Copy");
      return false;
    }

    return true;
  }();

  const auto& currentFrame = GetCurrentFrame();

  // TODO(vasumahesh):[TEXTURE]: VkScopedImage
  // TODO(vasumahesh):[LINT]: Remove Lint overrides
  const VkImage srcImage = m_swapChain.m_images[currentFrame];
  const VkImage dstImage = VkCore::CreateImage(m_device, storageFormat, ImageType::Image2D,
                                               Bounds2D{m_swapChain.m_extent.width, m_swapChain.m_extent.height}, 1, 1,
                                               1,
                                               VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                                               log_VulkanRenderSystem);

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(m_device, dstImage, &memRequirements);

  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize       = memRequirements.size;
  allocInfo.memoryTypeIndex      = VkCore::FindMemoryType(memRequirements.memoryTypeBits,
                                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, memProperties);

  VERIFY_VK_OP(log_VulkanRenderSystem, vkAllocateMemory(m_device, &allocInfo, nullptr, &dstMemory),
    "Snapshot: Unable to allocate Texture Memory for snapshot");
  VERIFY_VK_OP(log_VulkanRenderSystem, vkBindImageMemory(m_device, dstImage, dstMemory, 0),
    "Snapshot: Failed to bind Image and Image Memory");

  VkCommandBuffer snapshotCmdBuffer = VkCore::CreateCommandBuffer(m_device, m_graphicsCommandPool,
                                                                  VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                                                  log_VulkanRenderSystem);
  VkCore::BeginCommandBuffer(snapshotCmdBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, log_VulkanRenderSystem);

  VkCore::TransitionImageLayout(
                                snapshotCmdBuffer,
                                dstImage,
                                0,
                                VK_ACCESS_TRANSFER_WRITE_BIT,
                                VK_IMAGE_LAYOUT_UNDEFINED,
                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                VK_PIPELINE_STAGE_TRANSFER_BIT,
                                VK_PIPELINE_STAGE_TRANSFER_BIT,
                                VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

  // Transition swapchain image from present to transfer source layout
  VkCore::TransitionImageLayout(
                                snapshotCmdBuffer,
                                srcImage,
                                VK_ACCESS_MEMORY_READ_BIT,
                                VK_ACCESS_TRANSFER_READ_BIT,
                                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                VK_PIPELINE_STAGE_TRANSFER_BIT,
                                VK_PIPELINE_STAGE_TRANSFER_BIT,
                                VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
                               );

  if (supportsBlit) {
    VkCore::ImageBlit(snapshotCmdBuffer, srcImage, dstImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_ASPECT_COLOR_BIT,
                      Bounds3D{m_swapChain.m_extent.width, m_swapChain.m_extent.height, 1});
  } else {
    VkCore::ImageCopy(snapshotCmdBuffer, srcImage, dstImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_ASPECT_COLOR_BIT,
                      {m_swapChain.m_extent.width, m_swapChain.m_extent.height, 1});
  }

  VkCore::TransitionImageLayout(
                                snapshotCmdBuffer,
                                dstImage,
                                VK_ACCESS_TRANSFER_WRITE_BIT,
                                VK_ACCESS_MEMORY_READ_BIT,
                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                VK_IMAGE_LAYOUT_GENERAL,
                                VK_PIPELINE_STAGE_TRANSFER_BIT,
                                VK_PIPELINE_STAGE_TRANSFER_BIT,
                                VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
                               );

  // Transition source image back to its original layout
  VkCore::TransitionImageLayout(
                                snapshotCmdBuffer,
                                srcImage,
                                VK_ACCESS_TRANSFER_READ_BIT,
                                VK_ACCESS_MEMORY_READ_BIT,
                                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                VK_PIPELINE_STAGE_TRANSFER_BIT,
                                VK_PIPELINE_STAGE_TRANSFER_BIT,
                                VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
                               );

  VkCore::FlushCommandBuffer(m_device, snapshotCmdBuffer, m_graphicsQueue, log_VulkanRenderSystem);
  vkFreeCommandBuffers(m_device, m_graphicsCommandPool, 1, &snapshotCmdBuffer);

  // Get layout of the image (including row pitch)
  VkImageSubresource subResource{VK_IMAGE_ASPECT_COLOR_BIT, 0, 0};
  VkSubresourceLayout subResourceLayout;
  vkGetImageSubresourceLayout(m_device, dstImage, &subResource, &subResourceLayout);

  const char* data;
  vkMapMemory(m_device, dstMemory, 0, VK_WHOLE_SIZE, 0, (void**)&data); // NOLINT
  data += subResourceLayout.offset;                                     // NOLINT

  const U32 imageSize = m_swapChain.m_extent.width * m_swapChain.m_extent.height * (GetFormatSize(storageFormat));
  std::vector<char> imageData(imageSize);

  memcpy(imageData.data(), data, imageSize);

  std::ofstream file(exportPath, std::ios::out | std::ios::binary);
  file.write(&imageData[0], imageData.size());

  file.close();

  vkUnmapMemory(m_device, dstMemory);
  vkFreeMemory(m_device, dstMemory, nullptr);
  vkDestroyImage(m_device, dstImage, nullptr);

  LOG_INF(log_VulkanRenderSystem, LOG_LEVEL, "Snapshot Saved: Size: %d x %d", m_swapChain.m_extent.width, m_swapChain.
    m_extent.height);
}

} // namespace Vulkan
} // namespace Azura
