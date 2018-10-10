#pragma once
#include <vulkan/vulkan_core.h>
#include "Log/Log.h"
#include "Generic/GenericTypes.h"
#include "VkScopedImage.h"
#include "VkShader.h"

namespace Azura {
namespace Vulkan {
class VkScopedSwapChain;

class VkScopedRenderPass {
public:
  VkScopedRenderPass(Memory::Allocator& mainAllocator, Log logger);

  void Create(VkDevice device,
              const PipelinePassCreateInfo& createInfo,
              const Containers::Vector<RenderTargetCreateInfo>& pipelineBuffers,
              const Containers::Vector<VkScopedImage>& pipelineBufferImages,
              const Containers::Vector<VkShader>& allShaders,
              const VkScopedSwapChain& swapChain);

  VkRenderPass GetRenderPass() const;
  VkFramebuffer GetFrameBuffer() const;

  const Containers::Vector<VkPipelineShaderStageCreateInfo>& GetShaderStageInfo() const;

  void CleanUp() const;

private:
  Log log_VulkanRenderSystem;
  VkDevice m_device{};

  VkFramebuffer m_frameBuffer;

  VkRenderPass m_renderPass{};
  Containers::Vector<VkPipelineShaderStageCreateInfo> m_shaderPipelineInfos;
};

} // namespace Vulkan
} // namespace Azura
