#pragma once
#include <vulkan/vulkan_core.h>
#include "Log/Log.h"
#include "Generic/GenericTypes.h"
#include "VkScopedImage.h"

namespace Azura {
namespace Vulkan {

class VkScopedRenderPass {
public:
  VkScopedRenderPass(VkDevice device,
                     const PipelinePassCreateInfo& createInfo,
                     const Containers::Vector<RenderPassBufferCreateInfo>& pipelineBuffers,
                     Memory::Allocator& mainAllocator,
                     Log logger);

  VkRenderPass GetRenderPass() const;
  VkFramebuffer GetFrameBuffer() const;

private:
  Log log_VulkanRenderSystem;
  VkDevice m_device;

  VkFramebuffer m_frameBuffer;
  Containers::Vector<VkScopedImage> m_attachments;

  VkRenderPass m_renderPass;

};

} // namespace Vulkan
} // namespace Azura
