#include "Vulkan/VkScopedRenderPass.h"
#include "Vulkan/VkTypeMapping.h"
#include "Vulkan/VkCore.h"
#include "Memory/MemoryFactory.h"
#include "Vulkan/VkMacros.h"

namespace Azura {
namespace Vulkan {

using namespace Containers; // NOLINT

VkScopedRenderPass::VkScopedRenderPass(VkDevice device,
                                       const PipelinePassCreateInfo& createInfo,
                                       const Containers::Vector<RenderPassBufferCreateInfo>& pipelineBuffers,
                                       Memory::Allocator& mainAllocator,
                                       Log logger)
  : log_VulkanRenderSystem(logger),
    m_device(device),
    m_attachments(U32(createInfo.m_outputs.size()), mainAllocator) {

  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 2048);

  Vector<VkAttachmentReference> colorReferences{createInfo.m_outputs.size(), allocatorTemporary};
  VkAttachmentReference depthReference;
  Vector<VkAttachmentDescription> attachments{createInfo.m_outputs.size(), allocatorTemporary};

  U32 refCount = 0;
  bool hasDepth = false;

  for(const auto& output : createInfo.m_outputs)
  {
    RenderPassBufferCreateInfo* selected = nullptr;
    for(auto& buffer : pipelineBuffers)
    {
      if (buffer.m_id == output)
      {
        selected = &buffer;
        break;
      }
    }

    if (selected == nullptr)
    {
      LOG_ERR(log_VulkanRenderSystem, LOG_LEVEL, "Unknown Output in Render Pass supplied");
      continue;
    }

    const auto vkFormat = VkCore::GetVkFormat(selected->m_colorFormat, log_VulkanRenderSystem);

    VkAttachmentDescription attachmentDescription = {};
    attachmentDescription.format                  = vkFormat;
    attachmentDescription.samples                 = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescription.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescription.storeOp                 = VK_ATTACHMENT_STORE_OP_STORE;

    // Not using Stencil buffer
    attachmentDescription.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    if (HasDepthComponent(selected->m_colorFormat) || HasStencilComponent(selected->m_colorFormat))
    {
      // Record Depth Reference
      depthReference = VkAttachmentReference{ refCount, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

      hasDepth = true;

      // Shader Read for Color
      attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      attachmentDescription.finalLayout   = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }
    else
    {
      // Record Color Reference
      colorReferences.PushBack({ refCount, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });

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

}

VkRenderPass VkScopedRenderPass::GetRenderPass() const {
  return m_renderPass;
}

VkFramebuffer VkScopedRenderPass::GetFrameBuffer() const {
  return m_frameBuffer;
}
} // namespace Vulkan
} // namespace Azura
