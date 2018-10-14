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
                       const RenderPassRequirements& renderPassRequirements,
                       const DescriptorRequirements& descriptorRequirements,
                       const ShaderRequirements& shaderRequirements,
                       Memory::Allocator& mainAllocator,
                       Memory::Allocator& drawAllocator,
                       Window& window)
  : Renderer(appInfo, deviceRequirements, appRequirements, swapChainRequirement, descriptorRequirements, mainAllocator),
    log_VulkanRenderSystem(Log("VulkanRenderSystem")),
    m_perFrameBuffer(4096),
    m_perFrameAllocator(m_perFrameBuffer, 4096),
    m_window(window),
    m_drawablePools(renderPassRequirements.m_maxPools, drawAllocator),
    m_swapChain(mainAllocator, log_VulkanRenderSystem),
    m_renderPasses(renderPassRequirements.m_passSequence.GetSize(), mainAllocator),
    m_descriptorSetLayouts(mainAllocator),
    m_imageAvailableSemaphores(mainAllocator),
    m_renderFinishedSemaphores(mainAllocator),
    m_inFlightFences(mainAllocator),
    m_shaders(shaderRequirements.m_shaders.GetSize(), mainAllocator),
    m_renderPassAttachmentImages(renderPassRequirements.m_targets.GetSize(), mainAllocator),
    m_mainAllocator(mainAllocator),
    m_drawPoolAllocator(drawAllocator) {
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

  for (const auto& shaderCreateInfo : shaderRequirements.m_shaders) {
    VkRenderer::AddShader(shaderCreateInfo);
  }

  m_graphicsQueue = VkCore::GetQueueFromDevice(m_device, m_queueIndices.m_graphicsFamily);
  m_presentQueue  = VkCore::GetQueueFromDevice(m_device, m_queueIndices.m_presentFamily);

  const SwapChainDeviceSupport swapChainDeviceSupport =
    VkCore::QuerySwapChainSupport(m_physicalDevice, m_surface, allocatorTemporary);

  if (m_queueIndices.m_isTransferQueueRequired) {
    m_transferQueue = VkCore::GetQueueFromDevice(m_device, m_queueIndices.m_transferFamily);
  }

  m_graphicsCommandPool = VkCore::CreateCommandPool(m_device, m_queueIndices.m_graphicsFamily, 0,
                                                    log_VulkanRenderSystem);

  if (m_queueIndices.m_isTransferQueueRequired) {
    m_transferCommandPool =
      VkCore::CreateCommandPool(m_device, m_queueIndices.m_transferFamily, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
                                log_VulkanRenderSystem);
  }

  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

  m_swapChain.Create(m_device, m_physicalDevice, m_graphicsQueue, m_graphicsCommandPool, m_surface, m_queueIndices,
                     swapChainDeviceSupport, swapChainRequirement, memProperties);

  for (const auto& bufferCreateInfo : renderPassRequirements.m_targets) {
    TextureDesc desc = {};
    desc.m_format    = bufferCreateInfo.m_format;
    desc.m_bounds    = Bounds3D{m_swapChain.GetExtent().width, m_swapChain.GetExtent().height, 1};

    VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

    if (HasDepthComponent(bufferCreateInfo.m_format) || HasStencilComponent(bufferCreateInfo.m_format))
    {
      usageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    }

    LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Creating Attachment Input at: %d for %s", m_renderPassAttachmentImages.GetSize(), ToString(bufferCreateInfo.m_format).c_str());

    m_renderPassAttachmentImages.PushBack(VkScopedImage(m_device, desc,
                                                        usageFlags,
                                                        memProperties, log_VulkanRenderSystem));

    m_renderPassAttachmentImages.Last().CreateImageView(ImageViewType::ImageView2D);
  }

  U32 passCount = 0;
  for (const auto& passCreateInfo : renderPassRequirements.m_passSequence) {
    VkScopedRenderPass renderPass = VkScopedRenderPass(m_renderPasses.GetSize(), mainAllocator, log_VulkanRenderSystem);

    if (passCount != (renderPassRequirements.m_passSequence.GetSize() - 1)) {
      renderPass.Create(m_device,
                        m_graphicsCommandPool,
                        passCreateInfo,
                        renderPassRequirements.m_targets,
                        m_renderPassAttachmentImages,
                        m_shaders,
                        m_swapChain);
    } else {
      renderPass.CreateForSwapChain(m_device,
                                    m_graphicsCommandPool,
                                    passCreateInfo,
                                    m_shaders,
                                    m_swapChain);
    }

    m_renderPasses.PushBack(renderPass);
    ++passCount;
  }

  CreateDescriptorInfo();

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

  for (const auto& shader : m_shaders) {
    shader.CleanUp(m_device);
  }

  for (const auto& setLayout : m_descriptorSetLayouts) {
    vkDestroyDescriptorSetLayout(m_device, setLayout, nullptr);
  }

  vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);

  vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);

  m_swapChain.CleanUp(m_device);

  for (const auto& attachments : m_renderPassAttachmentImages) {
    attachments.CleanUp();
  }

  for (const auto& renderPass : m_renderPasses) {
    renderPass.CleanUp(m_device, m_graphicsCommandPool);
  }

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
                                       m_graphicsCommandPool,
                                       m_pipelineLayout, m_descriptorPool, m_descriptorSetLayouts,
                                       m_renderPasses, m_renderPassAttachmentImages, m_shaders,
                                       GetApplicationRequirements(), m_window.GetViewport(), memProperties,
                                       m_physicalDeviceProperties,
                                       m_swapChain, m_descriptorSlots, m_descriptorCount, m_drawPoolAllocator,
                                       m_mainAllocator, log_VulkanRenderSystem);

  m_drawablePools.PushBack(std::move(pool));

  return m_drawablePools.Last();
}

VkDevice VkRenderer::GetDevice() const {
  return m_device;
}

String VkRenderer::GetRenderingAPI() const {
  return "Vulkan";
}

void VkRenderer::Submit() {
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 4096);

  Vector<Vector<VkCommandBuffer>> secondaryCmdBuffers(m_renderPasses.GetSize(), allocatorTemporary);

  for (U32 idx = 0; idx < m_renderPasses.GetSize(); ++idx) {
    Vector<VkCommandBuffer> poolCmdBuffers(m_drawablePools.GetSize(), allocatorTemporary);
    secondaryCmdBuffers.PushBack(poolCmdBuffers);
  }

  for (auto& drawablePool : m_drawablePools) {
    drawablePool.Submit();
  }

  for (auto& drawablePool : m_drawablePools) {
    Vector<std::pair<U32, VkCommandBuffer>> drawableBuffer(allocatorTemporary);
    drawablePool.GetCommandBuffers(drawableBuffer);

    for (const auto& bufferPair : drawableBuffer) {
      secondaryCmdBuffers[bufferPair.first].PushBack(bufferPair.second);
    }
  }

  // Don't call last pass
  for (U32 idx             = 0; idx < m_renderPasses.GetSize() - 1; ++idx) {
    const auto& renderPass = m_renderPasses[idx];

    renderPass.Begin(m_swapChain);

    const auto& cmdBuffers = secondaryCmdBuffers[renderPass.GetId()];
    vkCmdExecuteCommands(renderPass.GetCommandBuffer(0), cmdBuffers.GetSize(), cmdBuffers.Data());

    renderPass.End();
  }

  const auto& lastPass         = m_renderPasses.Last();
  const auto& lastPassCommands = secondaryCmdBuffers.Last();

  lastPass.Begin(m_swapChain);
  for (U32 idx = 0; idx < lastPass.GetFrameBufferCount(); ++idx) {
    vkCmdExecuteCommands(lastPass.GetCommandBuffer(idx), lastPassCommands.GetSize(), lastPassCommands.Data());
  }
  lastPass.End();
}

void VkRenderer::CreateDescriptorInfo() {
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 4096);

  Vector<int> bindingSetSizes{m_descriptorSlots.GetSize(), allocatorTemporary};

  for (const auto& slot : m_descriptorSlots) {
    if (slot.m_binding == DescriptorBinding::Same) {
      bindingSetSizes.Last() += 1;
      continue;
    }

    bindingSetSizes.PushBack(1);
  }

  m_descriptorSetLayouts.Reserve(bindingSetSizes.GetSize() + m_renderPasses.GetSize());

  int offset = 0;
  for (const auto& bindingSize : bindingSetSizes) {
    Vector<VkDescriptorSetLayoutBinding> currentBindings(bindingSize, allocatorTemporary);

    for (int idx = offset; idx < offset + bindingSize; ++idx) {

      const auto& slot                  = m_descriptorSlots[idx];
      const auto combinedShaderFlagBits = GetCombinedShaderStageFlag(slot.m_stages);

      const auto bindingId = U32(idx - offset);

      switch (slot.m_type) {
        case DescriptorType::UniformBuffer:
          VkCore::CreateUniformBufferBinding(currentBindings, bindingId, 1, combinedShaderFlagBits);
          break;

        case DescriptorType::Sampler:
          VkCore::CreateSamplerBinding(currentBindings, bindingId, 1, combinedShaderFlagBits);
          break;

        case DescriptorType::SampledImage:
          VkCore::CreateSampledImageBinding(currentBindings, bindingId, 1, combinedShaderFlagBits);
          break;

        case DescriptorType::PushConstant:
        case DescriptorType::CombinedImageSampler:
        default:
          LOG_ERR(log_VulkanRenderSystem, LOG_LEVEL, "Unknown Descriptor Type");
          break;
      }
    }

    m_descriptorSetLayouts.
      PushBack(VkCore::CreateDescriptorSetLayout(m_device, currentBindings, log_VulkanRenderSystem));

    offset += bindingSize;
  }

  for (auto& renderPass : m_renderPasses) {
    const auto& setLayout = renderPass.GetDescriptorSetLayout();
    if (setLayout != VK_NULL_HANDLE) {
      renderPass.SetDescriptorSetId(m_descriptorSetLayouts.GetSize());
      m_descriptorSetLayouts.PushBack(setLayout);
    }
  }

  m_pipelineLayout = VkCore::CreatePipelineLayout(m_device, m_descriptorSetLayouts, log_VulkanRenderSystem);

  Vector<VkDescriptorPoolSize> descriptorPoolSizes(MAX_DESCRIPTOR_TYPE_COUNT, allocatorTemporary);

  // TODO(vasumahesh1):[DESCRIPTOR]: How to use Uniform Buffer Arrays?
  VkDescriptorPoolSize uniformPoolSize = {};
  uniformPoolSize.type                 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uniformPoolSize.descriptorCount      = m_descriptorCount.m_numUniformSlots;
  descriptorPoolSizes.PushBack(uniformPoolSize);

  if (m_descriptorCount.m_numSamplerSlots > 0) {
    VkDescriptorPoolSize samplerPoolSize = {};
    samplerPoolSize.type                 = VK_DESCRIPTOR_TYPE_SAMPLER;
    samplerPoolSize.descriptorCount      = m_descriptorCount.m_numSamplerSlots;
    descriptorPoolSizes.PushBack(samplerPoolSize);
  }

  if (m_descriptorCount.m_numSampledImageSlots > 0) {
    VkDescriptorPoolSize sampledImagePoolSize = {};
    sampledImagePoolSize.type                 = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    sampledImagePoolSize.descriptorCount      = m_descriptorCount.m_numSampledImageSlots;
    descriptorPoolSizes.PushBack(sampledImagePoolSize);
  }

  // TODO(vasumahesh1):[DESCRIPTOR]: 1 Set per Drawable? Need to Check
  VkDescriptorPoolCreateInfo poolInfo = {};
  poolInfo.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount              = descriptorPoolSizes.GetSize();
  poolInfo.pPoolSizes                 = descriptorPoolSizes.Data();

  // TODO(vasumahesh1):[DESCRIPTORS]: Max Sets issue!
  poolInfo.maxSets = m_drawablePools.GetMaxSize() * m_descriptorSetLayouts.GetSize();

  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool),
    "Unable to create Descriptor Pool");
}

void VkRenderer::RenderFrame() {
  EnterRenderFrame();

  const auto& currentFrame = GetCurrentFrame();

  vkWaitForFences(m_device, 1, &m_inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
  vkResetFences(m_device, 1, &m_inFlightFences[currentFrame]);

  U32 imageIndex;
  const VkResult result = vkAcquireNextImageKHR(m_device, m_swapChain.Real(), std::numeric_limits<uint64_t>::max(),
                                                m_imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    // TODO(vasumahesh1):[RESIZE]: Recreate Swapchain
    // RecreateSwapChain();
    return;
  }

  VERIFY_TRUE(log_VulkanRenderSystem, result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR,
    "Failed to acquire swap chain image");

  // std::array<VkSemaphore, 1> initialWaitSemaphores      = {};
  // std::array<VkSemaphore, 1> finalSignalSemaphores = {m_renderFinishedSemaphores[currentFrame]};

  // std::array<VkPipelineStageFlags, 1> waitStages = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  for (U32 idx             = 0; idx < m_renderPasses.GetSize(); ++idx) {
    const auto& renderPass = m_renderPasses[idx];

    VkCommandBuffer passBuffer;
    VkFence waitFence = VK_NULL_HANDLE;
    Vector<VkSemaphore> waitSemaphores(2, m_perFrameAllocator);
    Vector<VkPipelineStageFlags> waitStages(2, m_perFrameAllocator);
    Vector<VkSemaphore> signalSemaphores(2, m_perFrameAllocator);

    // Start of Render
    if (idx == 0) {
      waitSemaphores.PushBack(m_imageAvailableSemaphores[currentFrame]);
      waitStages.PushBack(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

      if (idx != m_renderPasses.GetSize() - 1) {
        const auto& nextPass = m_renderPasses[idx + 1];
        signalSemaphores.PushBack(nextPass.GetRenderSemaphore());
      }
    }
      // Somewhere in Middle
    else if (idx < m_renderPasses.GetSize() - 1) {
      const auto& nextPass = m_renderPasses[idx + 1];

      waitSemaphores.PushBack(renderPass.GetRenderSemaphore());
      waitStages.PushBack(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
      signalSemaphores.PushBack(nextPass.GetRenderSemaphore());
    }

    passBuffer = renderPass.GetCommandBuffer(0);

    // End of Render
    if (idx == m_renderPasses.GetSize() - 1) {
      if (idx != 0) {
        waitSemaphores.PushBack(renderPass.GetRenderSemaphore());
        waitStages.PushBack(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
      }

      signalSemaphores.PushBack(m_renderFinishedSemaphores[currentFrame]);

      passBuffer = renderPass.GetCommandBuffer(imageIndex);
      waitFence  = m_inFlightFences[currentFrame];
    }

    // SUCCESS OR SUBOPTIMAL
    VkSubmitInfo submitInfo         = {};
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount   = waitSemaphores.GetSize();
    submitInfo.pWaitSemaphores      = waitSemaphores.Data();
    submitInfo.pWaitDstStageMask    = waitStages.Data();
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &passBuffer;
    submitInfo.signalSemaphoreCount = signalSemaphores.GetSize();
    submitInfo.pSignalSemaphores    = signalSemaphores.Data();

    VERIFY_VK_OP(log_VulkanRenderSystem, vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, waitFence),
      "Failed to submit draw command buffer");
  }

  Vector<VkSemaphore> presentWaitSemaphores({m_renderFinishedSemaphores[currentFrame]}, m_perFrameAllocator);

  VkPresentInfoKHR presentInfo   = {};
  presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = presentWaitSemaphores.GetSize();
  presentInfo.pWaitSemaphores    = presentWaitSemaphores.Data();

  std::array<VkSwapchainKHR, 1> swapChains = {m_swapChain.Real()};
  presentInfo.swapchainCount               = U32(swapChains.size());
  presentInfo.pSwapchains                  = swapChains.data();
  presentInfo.pImageIndices                = &imageIndex;
  presentInfo.pResults                     = nullptr;

  vkQueuePresentKHR(m_presentQueue, &presentInfo);

  m_perFrameAllocator.Reset();

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
    vkGetPhysicalDeviceFormatProperties(m_physicalDevice, m_swapChain.GetSurfaceFormat(), &formatProps);
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
  const VkScopedImage& srcImage = m_swapChain.GetImage(currentFrame);

  const auto swapChainExtent = m_swapChain.GetExtent();
  const VkImage dstImage     = VkCore::CreateImage(m_device, storageFormat, ImageType::Image2D,
                                                   Bounds2D{swapChainExtent.width, swapChainExtent.height}, 1, 1,
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
                                srcImage.Real(),
                                VK_ACCESS_MEMORY_READ_BIT,
                                VK_ACCESS_TRANSFER_READ_BIT,
                                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                VK_PIPELINE_STAGE_TRANSFER_BIT,
                                VK_PIPELINE_STAGE_TRANSFER_BIT,
                                VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
                               );

  if (supportsBlit) {
    VkCore::ImageBlit(snapshotCmdBuffer, srcImage.Real(), dstImage, VK_IMAGE_ASPECT_COLOR_BIT,
                      VK_IMAGE_ASPECT_COLOR_BIT,
                      Bounds3D{swapChainExtent.width, swapChainExtent.height, 1});
  } else {
    VkCore::ImageCopy(snapshotCmdBuffer, srcImage.Real(), dstImage, VK_IMAGE_ASPECT_COLOR_BIT,
                      VK_IMAGE_ASPECT_COLOR_BIT,
                      {swapChainExtent.width, swapChainExtent.height, 1});
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
                                srcImage.Real(),
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

  const U32 imageSize = swapChainExtent.width * swapChainExtent.height * (GetFormatSize(storageFormat));
  std::vector<char> imageData(imageSize);

  memcpy(imageData.data(), data, imageSize);

  std::ofstream file(exportPath, std::ios::out | std::ios::binary);
  file.write(&imageData[0], imageData.size());

  file.close();

  vkUnmapMemory(m_device, dstMemory);
  vkFreeMemory(m_device, dstMemory, nullptr);
  vkDestroyImage(m_device, dstImage, nullptr);

  LOG_INF(log_VulkanRenderSystem, LOG_LEVEL, "Snapshot Saved: Size: %d x %d", swapChainExtent.width, swapChainExtent.
    height);
}

void VkRenderer::AddShader(const ShaderCreateInfo& info) {
  // TODO(vasumahesh1):[ASSETS]: Manage assets
  const String fullPath = "Shaders/" + VkRenderer::GetRenderingAPI() + "/" + info.m_shaderFileName;
  m_shaders.EmplaceBack(m_device, fullPath, log_VulkanRenderSystem);
  m_shaders.Last().SetStage(info.m_stage);
}

} // namespace Vulkan
} // namespace Azura
