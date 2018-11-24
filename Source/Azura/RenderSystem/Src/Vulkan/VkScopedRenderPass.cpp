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
    m_descriptorSetLayout(VK_NULL_HANDLE),
    m_frameBuffers(mainAllocator),
    m_commandBuffers(mainAllocator),
    m_inputAttachments(mainAllocator),
    m_colorBlendAttachments(mainAllocator),
    m_clearValues(mainAllocator),
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

  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Creating Render Pass: ID %d", m_id);

  CreateDescriptorSetLayout(device, createInfo);

  m_shaderPipelineInfos.Reserve(U32(createInfo.m_shaders.size()));

  for (const auto& vkShaderId : createInfo.m_shaders) {
    const auto& vkShader = allShaders[vkShaderId];
    m_shaderPipelineInfos.PushBack(vkShader.GetShaderStageInfo());
  }

  Vector<VkAttachmentReference> colorReferences{U32(createInfo.m_outputTargets.size()), allocatorTemporary};
  VkAttachmentReference depthReference;
  Vector<VkAttachmentDescription> attachments{U32(createInfo.m_outputTargets.size()), allocatorTemporary};
  Vector<VkImageView> attachmentViews{U32(createInfo.m_outputTargets.size()), allocatorTemporary};
  m_colorBlendAttachments.Reserve(U32(createInfo.m_outputTargets.size()));
  m_clearValues.Reserve(U32(createInfo.m_outputTargets.size()));

  U32 refCount  = 0;
  bool hasDepth = false;

  for (const auto& output : createInfo.m_outputTargets) {
    const auto& selected = pipelineBuffers[output];

    VkClearValue clearValue = {};

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

      clearValue.depthStencil = { createInfo.m_clearData.m_depth, createInfo.m_clearData.m_stencil };
      m_clearValues.PushBack(clearValue);

    } else {
      // Record Color Reference
      colorReferences.PushBack({refCount, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});

      // Shader Read for Color
      attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      attachmentDescription.finalLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

      VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
      colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
      colorBlendAttachment.blendEnable = VkBool32(createInfo.m_blendState.m_enable);

      if (createInfo.m_blendState.m_enable) {
        LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Enabling Blend State For Render Pass");

        const auto colorSrcFactor = ToVkBlendFactor(createInfo.m_blendState.m_color.m_srcFactor);
        VERIFY_OPT(log_VulkanRenderSystem, colorSrcFactor, "Invalid colorSrcFactor converted for Blend State");

        const auto colorDstFactor = ToVkBlendFactor(createInfo.m_blendState.m_color.m_dstFactor);
        VERIFY_OPT(log_VulkanRenderSystem, colorDstFactor, "Invalid colorDstFactor converted for Blend State");

        const auto colorOp = ToVkBlendOp(createInfo.m_blendState.m_color.m_op);
        VERIFY_OPT(log_VulkanRenderSystem, colorOp, "Invalid colorOp converted for Blend State");

        const auto alphaSrcFactor = ToVkBlendFactor(createInfo.m_blendState.m_alpha.m_srcFactor);
        VERIFY_OPT(log_VulkanRenderSystem, alphaSrcFactor, "Invalid alphaSrcFactor converted for Blend State");

        const auto alphaDstFactor = ToVkBlendFactor(createInfo.m_blendState.m_alpha.m_dstFactor);
        VERIFY_OPT(log_VulkanRenderSystem, alphaDstFactor, "Invalid alphaDstFactor converted for Blend State");

        const auto alphaOp = ToVkBlendOp(createInfo.m_blendState.m_alpha.m_op);
        VERIFY_OPT(log_VulkanRenderSystem, alphaOp, "Invalid alphaOp converted for Blend State");

        colorBlendAttachment.srcColorBlendFactor = colorSrcFactor.value();
        colorBlendAttachment.dstColorBlendFactor = colorDstFactor.value();
        colorBlendAttachment.colorBlendOp = colorOp.value();

        colorBlendAttachment.srcAlphaBlendFactor = alphaSrcFactor.value();
        colorBlendAttachment.dstAlphaBlendFactor = alphaDstFactor.value();
        colorBlendAttachment.alphaBlendOp = alphaOp.value();
      }

      m_colorBlendAttachments.PushBack(colorBlendAttachment);

      clearValue.color = { createInfo.m_clearData.m_color[0], createInfo.m_clearData.m_color[1], createInfo.m_clearData.m_color[2], createInfo.m_clearData.m_color[3] };
      m_clearValues.PushBack(clearValue);
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

  m_numAttachments = attachmentViews.GetSize();

  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateFramebuffer(device, &framebufferInfo, nullptr, m_frameBuffers.Data()),
    "Failed to create single framebuffer");

  m_commandBuffers.PushBack(VkCore::CreateCommandBuffer(device, commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                                        log_VulkanRenderSystem));

  m_beginRenderSemaphore = VkCore::CreateSemaphore(device, log_VulkanRenderSystem);
}

void VkScopedRenderPass::CreateForSwapChain(VkDevice device,
                                            VkCommandPool commandPool,
                                            const PipelinePassCreateInfo& createInfo,
                                            const Vector<VkShader>& allShaders,
                                            const VkScopedSwapChain& swapChain) {
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 2048);

  CreateDescriptorSetLayout(device, createInfo);

  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Creating Render Pass (Swap Chain): ID %d", m_id);

  m_shaderPipelineInfos.Reserve(U32(createInfo.m_shaders.size()));
  m_clearValues.Reserve(2);

  VkClearValue clearValue;

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

  m_colorBlendAttachments.Reserve(1);

  VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VkBool32(createInfo.m_blendState.m_enable);

  if (createInfo.m_blendState.m_enable) {
    LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Enabling Blend State For Render Pass");

    const auto colorSrcFactor = ToVkBlendFactor(createInfo.m_blendState.m_color.m_srcFactor);
    VERIFY_OPT(log_VulkanRenderSystem, colorSrcFactor, "Invalid colorSrcFactor converted for Blend State");

    const auto colorDstFactor = ToVkBlendFactor(createInfo.m_blendState.m_color.m_dstFactor);
    VERIFY_OPT(log_VulkanRenderSystem, colorDstFactor, "Invalid colorDstFactor converted for Blend State");

    const auto colorOp = ToVkBlendOp(createInfo.m_blendState.m_color.m_op);
    VERIFY_OPT(log_VulkanRenderSystem, colorOp, "Invalid colorOp converted for Blend State");

    const auto alphaSrcFactor = ToVkBlendFactor(createInfo.m_blendState.m_alpha.m_srcFactor);
    VERIFY_OPT(log_VulkanRenderSystem, alphaSrcFactor, "Invalid alphaSrcFactor converted for Blend State");

    const auto alphaDstFactor = ToVkBlendFactor(createInfo.m_blendState.m_alpha.m_dstFactor);
    VERIFY_OPT(log_VulkanRenderSystem, alphaDstFactor, "Invalid alphaDstFactor converted for Blend State");

    const auto alphaOp = ToVkBlendOp(createInfo.m_blendState.m_alpha.m_op);
    VERIFY_OPT(log_VulkanRenderSystem, alphaOp, "Invalid alphaOp converted for Blend State");

    colorBlendAttachment.srcColorBlendFactor = colorSrcFactor.value();
    colorBlendAttachment.dstColorBlendFactor = colorDstFactor.value();
    colorBlendAttachment.colorBlendOp = colorOp.value();

    colorBlendAttachment.srcAlphaBlendFactor = alphaSrcFactor.value();
    colorBlendAttachment.dstAlphaBlendFactor = alphaDstFactor.value();
    colorBlendAttachment.alphaBlendOp = alphaOp.value();
  }

  m_colorBlendAttachments.PushBack(colorBlendAttachment);

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
  subpass.pDepthStencilAttachment = swapChain.HasDepthSupport() ? &depthAttachmentRef : nullptr;

  Vector<VkAttachmentDescription> attachments{ 2, allocatorTemporary };
  attachments.PushBack(colorAttachment);

  clearValue.color = { createInfo.m_clearData.m_color[0], createInfo.m_clearData.m_color[1], createInfo.m_clearData.m_color[2], createInfo.m_clearData.m_color[3] };
  m_clearValues.PushBack(clearValue);

  if (swapChain.HasDepthSupport())
  {
    attachments.PushBack(depthAttachment);

    clearValue.depthStencil = { createInfo.m_clearData.m_depth, createInfo.m_clearData.m_stencil };
    m_clearValues.PushBack(clearValue);
  }

  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount        = attachments.GetSize();
  renderPassInfo.pAttachments           = attachments.Data();
  renderPassInfo.subpassCount           = 1;
  renderPassInfo.pSubpasses             = &subpass;

  m_numAttachments = attachments.GetSize();

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
    Vector<VkImageView> swapAttachments{ 2, allocatorTemporary };
    swapAttachments.PushBack(allImages[idx].View());

    if (swapChain.HasDepthSupport())
    {
      swapAttachments.PushBack(depthImageView);
    }

    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass              = m_renderPass;
    framebufferInfo.attachmentCount         = swapAttachments.GetSize();
    framebufferInfo.pAttachments            = swapAttachments.Data();
    framebufferInfo.width                   = swapChainExtent.width;
    framebufferInfo.height                  = swapChainExtent.height;
    framebufferInfo.layers                  = 1;

    VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_frameBuffers[idx]),
      "Failed to create framebuffer");
  }

  m_commandBuffers.Resize(m_frameBuffers.GetSize());
  VkCore::CreateCommandBuffers(device, commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                               m_commandBuffers, log_VulkanRenderSystem);

  m_beginRenderSemaphore = VkCore::CreateSemaphore(device, log_VulkanRenderSystem);
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

const Containers::Vector<PipelinePassInput>& VkScopedRenderPass::GetPassInputs() const {
  return m_inputAttachments;
}

U32 VkScopedRenderPass::GetFrameBufferCount() const {
  return m_frameBuffers.GetSize();
}

VkSemaphore VkScopedRenderPass::GetRenderSemaphore() const {
  return m_beginRenderSemaphore;
}

VkDescriptorSetLayout VkScopedRenderPass::GetDescriptorSetLayout() const {
  return m_descriptorSetLayout;
}

U32 VkScopedRenderPass::GetId() const {
  return m_id;
}

U32 VkScopedRenderPass::GetDescriptorSetId() const {
  return m_descriptorSet;
}

void VkScopedRenderPass::SetDescriptorSetId(U32 id) {
  m_descriptorSet = id;
}

void VkScopedRenderPass::Begin(const VkScopedSwapChain& swapChain) const {
  for (U32 idx = 0; idx < m_frameBuffers.GetSize(); ++idx) {
    VkCore::BeginCommandBuffer(m_commandBuffers[idx], VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
                               log_VulkanRenderSystem);

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType                 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass            = m_renderPass;
    renderPassInfo.framebuffer           = m_frameBuffers[idx];
    renderPassInfo.renderArea.offset     = {0, 0};
    renderPassInfo.renderArea.extent     = swapChain.GetExtent();
    renderPassInfo.clearValueCount       = m_clearValues.GetSize();
    renderPassInfo.pClearValues          = m_clearValues.Data();

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

const Containers::Vector<VkPipelineColorBlendAttachmentState>& VkScopedRenderPass::GetColorBlendAttachments() const {
  return m_colorBlendAttachments;
}

void VkScopedRenderPass::CleanUp(VkDevice device, VkCommandPool commandPool) const {
  vkDestroyRenderPass(device, m_renderPass, nullptr);

  for (const auto& frameBuffer : m_frameBuffers) {
    vkDestroyFramebuffer(device, frameBuffer, nullptr);
  }

  vkDestroySemaphore(device, m_beginRenderSemaphore, nullptr);

  vkFreeCommandBuffers(device, commandPool, m_commandBuffers.GetSize(), m_commandBuffers.Data());
}

void VkScopedRenderPass::CreateDescriptorSetLayout(VkDevice device, const PipelinePassCreateInfo& createInfo) {
  if (createInfo.m_inputTargets.empty()) {
    return;
  }

  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 1024);

  U32 bindingId = 0;

  m_inputAttachments.Reserve(U32(createInfo.m_inputTargets.size()));

  Vector<VkDescriptorSetLayoutBinding> currentBindings(U32(createInfo.m_inputTargets.size()), allocatorTemporary);
  for (const auto& input : createInfo.m_inputTargets) {
    m_inputAttachments.PushBack(input);

    const auto combinedShaderFlagBits = GetCombinedShaderStageFlag(input.m_stages);
    VkCore::CreateSampledImageBinding(currentBindings, bindingId, 1, combinedShaderFlagBits);

    ++bindingId;
  }

  m_descriptorSetLayout = VkCore::CreateDescriptorSetLayout(device, currentBindings, log_VulkanRenderSystem);
}
} // namespace Vulkan
} // namespace Azura
