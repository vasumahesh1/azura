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
  VkScopedRenderPass(U32 id, Memory::Allocator& mainAllocator, Log logger);

  void Create(VkDevice device,
              VkCommandPool commandPool,
              bool createFrameBuffer,
              const PipelinePassCreateInfo& createInfo,
              const Containers::Vector<RenderTargetCreateInfo>& pipelineBuffers,
              const Containers::Vector<VkScopedImage>& pipelineBufferImages,
              const Containers::Vector<VkShader>& allShaders,
              const VkScopedSwapChain& swapChain);

  VkRenderPass GetRenderPass() const;
  VkFramebuffer GetFrameBuffer() const;
  VkCommandBuffer GetCommandBuffer() const;

  U32 GetId() const;

  void Begin(const VkScopedSwapChain& swapChain, std::array<VkClearValue, 2> clearData) const;
  void End() const;

  const Containers::Vector<VkPipelineShaderStageCreateInfo>& GetShaderStageInfo() const;

  void CleanUp(VkDevice device, VkCommandPool commandPool) const;

private:
  Log log_VulkanRenderSystem;

  U32 m_id;

  VkFramebuffer m_frameBuffer;
  VkCommandBuffer m_commandBuffer{};

  VkRenderPass m_renderPass{};
  Containers::Vector<VkPipelineShaderStageCreateInfo> m_shaderPipelineInfos;
};

} // namespace Vulkan
} // namespace Azura
