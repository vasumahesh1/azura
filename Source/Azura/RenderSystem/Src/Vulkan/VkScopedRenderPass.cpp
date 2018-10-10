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

VkScopedRenderPass::VkScopedRenderPass(Memory::Allocator& mainAllocator, Log logger)
  : log_VulkanRenderSystem(std::move(logger)),
    m_frameBuffer(),
    m_shaderPipelineInfos(mainAllocator) {
}

void VkScopedRenderPass::Create(VkDevice device,
                                const PipelinePassCreateInfo& createInfo,
                                const Vector<RenderTargetCreateInfo>& pipelineBuffers,
                                const Vector<VkScopedImage>& pipelineBufferImages,
                                const Vector<VkShader>& allShaders,
                                const VkScopedSwapChain& swapChain) {
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 2048);

  m_device = device;

  m_shaderPipelineInfos.Reserve(U32(createInfo.m_shaders.size()));

  for (const auto& vkShaderId : createInfo.m_shaders)
  {
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
  VkSubpassDependency dependency = {};
  dependency.srcSubpass          = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass          = 0;
  dependency.srcStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask       = 0;
  dependency.dstStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies   = &dependency;

  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass),
    "Failed to create render pass");

  VkFramebufferCreateInfo framebufferInfo = {};
  framebufferInfo.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferInfo.renderPass              = m_renderPass;
  framebufferInfo.attachmentCount         = U32(attachmentViews.GetSize());
  framebufferInfo.pAttachments            = attachmentViews.Data();
  framebufferInfo.width                   = swapChain.GetExtent().width;
  framebufferInfo.height                  = swapChain.GetExtent().height;
  framebufferInfo.layers                  = 1;

  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_frameBuffer),
    "Failed to create single framebuffer");
}

VkRenderPass VkScopedRenderPass::GetRenderPass() const {
  return m_renderPass;
}

VkFramebuffer VkScopedRenderPass::GetFrameBuffer() const {
  return m_frameBuffer;
}

const Vector<VkPipelineShaderStageCreateInfo>& VkScopedRenderPass::GetShaderStageInfo() const {
  return m_shaderPipelineInfos;
}

void VkScopedRenderPass::CleanUp() const {
  vkDestroyRenderPass(m_device, m_renderPass, nullptr);
  vkDestroyFramebuffer(m_device, m_frameBuffer, nullptr);

}
} // namespace Vulkan
} // namespace Azura
