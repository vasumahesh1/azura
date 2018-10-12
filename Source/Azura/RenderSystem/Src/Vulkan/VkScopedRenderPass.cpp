#include <utility>
#include "Vulkan/VkScopedRenderPass.h"
#include "Vulkan/VkTypeMapping.h"
#include "Vulkan/VkCore.h"
#include "Memory/MemoryFactory.h"
#include "Vulkan/VkMacros.h"
#include "Vulkan/VkScopedSwapChain.h"

namespace Azura {
namespace Vulkan {

using namespace Containers; // NOLINT

VkScopedRenderPass::VkScopedRenderPass(U32 idx, Memory::Allocator& mainAllocator, Log logger)
  : log_VulkanRenderSystem(std::move(logger)),
    m_id(idx),
    m_frameBuffers(mainAllocator),
    m_commandBuffers(mainAllocator),
    m_shaderPipelineInfos(mainAllocator) {
}

void VkScopedRenderPass::Create(VkDevice device,
                                VkCommandPool commandPool,
                                const PipelinePassCreateInfo& createInfo,
                                const Vector<RenderTargetCreateInfo>& pipelineBuffers,
                                const Vector<VkScopedImage>& pipelineBufferImages,
                                const Vector<VkShader>& allShaders,
                                const VkScopedSwapChain& swapChain) {
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 2048);

  m_commandBuffers.Reserve(1);
  m_frameBuffers.Resize(1);

  m_shaderPipelineInfos.Reserve(U32(createInfo.m_shaders.size()));

  for (const auto& vkShaderId : createInfo.m_shaders) {
    const auto& vkShader = allShaders[vkShaderId];
    m_shaderPipelineInfos.PushBack(vkShader.GetShaderStageInfo());
  }

  Vector<VkAttachmentReference> colorReferences{U32(createInfo.m_outputs.size()), allocatorTemporary};
  VkAttachmentReference depthReference;
  Vector<VkAttachmentDescription> attachments{U32(createInfo.m_outputs.size()), allocatorTemporary};
  Vector<VkImageView> attachmentViews{U32(createInfo.m_outputs.size()), allocatorTemporary};

  U32 refCount  = 0;
  bool hasDepth = false;

  for (const auto& output : createInfo.m_outputs) {
    const auto& selected = pipelineBuffers[output];

    // Push View to Vector as we process output
    attachmentViews.PushBack(pipelineBufferImages[output].View());

    const auto vkFormat = VkCore::GetVkFormat(selected.m_format, log_VulkanRenderSystem);

    VkAttachmentDescription attachmentDescription = {};
    attachmentDescription.format                  = vkFormat;
    attachmentDescription.samples                 = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescription.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescription.storeOp                 = VK_ATTACHMENT_STORE_OP_STORE;

    // Not using Stencil buffer
    attachmentDescription.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    if (HasDepthComponent(selected.m_format) || HasStencilComponent(selected.m_format)) {
      // Record Depth Reference
      depthReference = VkAttachmentReference{refCount, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

      hasDepth = true;

      // Shader Read for Color
      attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      attachmentDescription.finalLayout   = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    } else {
      // Record Color Reference
      colorReferences.PushBack({refCount, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});

      // Shader Read for Color
      attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      attachmentDescription.finalLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }

    attachments.PushBack(attachmentDescription);

    ++refCount;
  }

  VkSubpassDescription subpass    = {};
  subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount    = colorReferences.GetSize();
  subpass.pColorAttachments       = colorReferences.Data();
  subpass.pDepthStencilAttachment = hasDepth ? &depthReference : nullptr;

  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount        = attachments.GetSize();
  renderPassInfo.pAttachments           = attachments.Data();
  renderPassInfo.subpassCount           = 1;
  renderPassInfo.pSubpasses             = &subpass;

  // Tell special subpass to wait for Image acquisition from semaphore
  std::array<VkSubpassDependency, 2> dependencies = {};

  dependencies[0].srcSubpass    = VK_SUBPASS_EXTERNAL;
  dependencies[0].dstSubpass    = 0;
  dependencies[0].srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependencies[0].srcAccessMask = 0;
  dependencies[0].dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  dependencies[1].srcSubpass      = 0;
  dependencies[1].dstSubpass      = VK_SUBPASS_EXTERNAL;
  dependencies[1].srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependencies[1].dstStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  dependencies[1].srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependencies[1].dstAccessMask   = VK_ACCESS_MEMORY_READ_BIT;
  dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  renderPassInfo.dependencyCount = U32(dependencies.size());
  renderPassInfo.pDependencies   = dependencies.data();

  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_renderPass),
    "Failed to create render pass");

  VkFramebufferCreateInfo framebufferInfo = {};
  framebufferInfo.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferInfo.renderPass              = m_renderPass;
  framebufferInfo.attachmentCount         = U32(attachmentViews.GetSize());
  framebufferInfo.pAttachments            = attachmentViews.Data();
  framebufferInfo.width                   = swapChain.GetExtent().width;
  framebufferInfo.height                  = swapChain.GetExtent().height;
  framebufferInfo.layers                  = 1;

  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateFramebuffer(device, &framebufferInfo, nullptr, m_frameBuffers.Data()),
    "Failed to create single framebuffer");

  m_commandBuffers.PushBack(VkCore::CreateCommandBuffer(device, commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                                log_VulkanRenderSystem));


  m_beginRenderSemaphore = VkCore::CreateSemaphore(device, log_VulkanRenderSystem);
}

void VkScopedRenderPass::CreateForSwapChain(VkDevice device,
  VkCommandPool commandPool,
  const PipelinePassCreateInfo& createInfo,
  const Vector<RenderTargetCreateInfo>& pipelineBuffers,
  const Vector<VkScopedImage>& pipelineBufferImages,
  const Vector<VkShader>& allShaders,
  const VkScopedSwapChain& swapChain) {
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 2048);

  m_shaderPipelineInfos.Reserve(U32(createInfo.m_shaders.size()));

  for (const auto& vkShaderId : createInfo.m_shaders) {
    const auto& vkShader = allShaders[vkShaderId];
    m_shaderPipelineInfos.PushBack(vkShader.GetShaderStageInfo());
  }

  VkAttachmentDescription colorAttachment = {};
  colorAttachment.format                  = swapChain.GetSurfaceFormat();
  colorAttachment.samples                 = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp                 = VK_ATTACHMENT_STORE_OP_STORE;

  // Not using Stencil buffer
  colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout   = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentDescription depthAttachment = {};
  depthAttachment.format                  = swapChain.GetDepthFormat();
  depthAttachment.samples                 = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp                 = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp           = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp          = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout             = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment            = 0;
  colorAttachmentRef.layout                = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentRef = {};
  depthAttachmentRef.attachment            = 1;
  depthAttachmentRef.layout                = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass    = {};
  subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount    = 1;
  subpass.pColorAttachments       = &colorAttachmentRef;
  subpass.pDepthStencilAttachment = &depthAttachmentRef;

  std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount        = U32(attachments.size());
  renderPassInfo.pAttachments           = attachments.data();
  renderPassInfo.subpassCount           = 1;
  renderPassInfo.pSubpasses             = &subpass;

  // Tell special subpass to wait for Image acquisition from semaphore
  std::array<VkSubpassDependency, 1> dependencies = {};

  dependencies[0].srcSubpass    = VK_SUBPASS_EXTERNAL;
  dependencies[0].dstSubpass    = 0;
  dependencies[0].srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependencies[0].srcAccessMask = 0;
  dependencies[0].dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  renderPassInfo.dependencyCount = U32(dependencies.size());
  renderPassInfo.pDependencies   = dependencies.data();

  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_renderPass),
    "Failed to create render pass");

  const auto& allImages      = swapChain.GetAllImages();
  const auto swapChainExtent = swapChain.GetExtent();

  m_frameBuffers.Resize(allImages.GetSize());

  const VkImageView depthImageView = swapChain.GetDepthImage().View();

  for (U32 idx = 0; idx < allImages.GetSize(); ++idx) {

    const std::array<VkImageView, 2> swapAttachments = {allImages[idx].View(), depthImageView};

    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass              = m_renderPass;
    framebufferInfo.attachmentCount         = U32(swapAttachments.size());
    framebufferInfo.pAttachments            = swapAttachments.data();
    framebufferInfo.width                   = swapChainExtent.width;
    framebufferInfo.height                  = swapChainExtent.height;
    framebufferInfo.layers                  = 1;

    VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_frameBuffers[idx]),
      "Failed to create framebuffer");
  }

  m_commandBuffers.Resize(m_frameBuffers.GetSize());
  VkCore::CreateCommandBuffers(device, commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    m_commandBuffers, log_VulkanRenderSystem);
}

VkRenderPass VkScopedRenderPass::GetRenderPass() const {
  return m_renderPass;
}

VkFramebuffer VkScopedRenderPass::GetFrameBuffer(U32 idx) const {
  return m_frameBuffers[idx];
}

VkCommandBuffer VkScopedRenderPass::GetCommandBuffer(U32 idx) const {
  return m_commandBuffers[idx];
}

U32 VkScopedRenderPass::GetFrameBufferCount() const {
  return m_frameBuffers.GetSize();
}

VkSemaphore VkScopedRenderPass::GetRenderSemaphore() const {
  return m_beginRenderSemaphore;
}

U32 VkScopedRenderPass::GetId() const {
  return m_id;
}

void VkScopedRenderPass::Begin(const VkScopedSwapChain& swapChain, std::array<VkClearValue, 2> clearData) const {

  for (U32 idx = 0; idx < m_frameBuffers.GetSize(); ++idx) {
    VkCore::BeginCommandBuffer(m_commandBuffers[idx], VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, log_VulkanRenderSystem);

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType                 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass            = m_renderPass;
    renderPassInfo.framebuffer           = m_frameBuffers[idx];
    renderPassInfo.renderArea.offset     = {0, 0};
    renderPassInfo.renderArea.extent     = swapChain.GetExtent();
    renderPassInfo.clearValueCount       = U32(clearData.size());
    renderPassInfo.pClearValues          = clearData.data();

    vkCmdBeginRenderPass(m_commandBuffers[idx], &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
  }
}

void VkScopedRenderPass::End() const {
  for (U32 idx = 0; idx < m_commandBuffers.GetSize(); ++idx) {
    vkCmdEndRenderPass(m_commandBuffers[idx]);
    VkCore::EndCommandBuffer(m_commandBuffers[idx], log_VulkanRenderSystem);
  }
}

const Vector<VkPipelineShaderStageCreateInfo>& VkScopedRenderPass::GetShaderStageInfo() const {
  return m_shaderPipelineInfos;
}

void VkScopedRenderPass::CleanUp(VkDevice device, VkCommandPool commandPool) const {
  vkDestroyRenderPass(device, m_renderPass, nullptr);

  for (const auto& frameBuffer : m_frameBuffers) {
    vkDestroyFramebuffer(device, frameBuffer, nullptr);
  }

  vkDestroySemaphore(device, m_beginRenderSemaphore, nullptr);

  vkFreeCommandBuffers(device, commandPool, m_commandBuffers.GetSize(), m_commandBuffers.Data());
}
} // namespace Vulkan
} // namespace Azura
