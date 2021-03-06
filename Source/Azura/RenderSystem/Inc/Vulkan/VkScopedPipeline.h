#pragma once
#include <vulkan/vulkan_core.h>

#include <map>
#include "Containers/Vector.h"
#include "Generic/GenericTypes.h"
#include "Vulkan/VkScopedSwapChain.h"
#include "Vulkan/VkScopedRenderPass.h"
#include "Log/Log.h"

namespace Azura {
namespace Vulkan {

class VkScopedPipeline {
 public:
  VkScopedPipeline(VkPipeline pipeline);
  VkPipeline Real() const;
  void CleanUp(VkDevice device) const;

private:
  VkPipeline m_pipeline;
};

class VkPipelineFactory {
 public:
  VkPipelineFactory(VkDevice device, Memory::Allocator& allocator, Log logger);

  VkPipelineFactory& AddBindingDescription(U32 stride, VertexSlot slot, U32 binding);

  VkPipelineFactory& BulkAddAttributeDescription(const VertexSlot& vertexSlot, U32 binding);

  VkPipelineFactory& SetInputAssemblyStage(PrimitiveTopology topology);
  VkPipelineFactory& SetViewportStage(ViewportDimensions viewportDimensions, const VkScopedSwapChain& swapChain);
  VkPipelineFactory& SetRasterizerStage(CullMode cullMode, FrontFace faceOrder);
  VkPipelineFactory& SetMultisampleStage();
  VkPipelineFactory& SetPipelineLayout(VkPipelineLayout layout);
  VkPipelineFactory& AddShaderStage(const VkPipelineShaderStageCreateInfo& shaderStageCreateInfo);

  void Submit(Containers::Vector<std::reference_wrapper<VkScopedRenderPass>> renderPasses, Containers::Vector<VkScopedPipeline>& result) const;

 private:
  struct BindingInfo {
    U32 m_offset{0};
  };

  const VkDevice m_device;

  // TODO(vasumahesh1): Make our own map
  std::map<U32, BindingInfo> m_bindingMap;

  Containers::Vector<VkVertexInputBindingDescription> m_bindingInfo;
  Containers::Vector<VkVertexInputAttributeDescription> m_attributeDescription;
  Containers::Vector<VkPipelineShaderStageCreateInfo> m_stages;

  VkPipelineInputAssemblyStateCreateInfo m_inputAssemblyStage{};
  VkPipelineViewportStateCreateInfo m_viewportStage{};
  VkPipelineRasterizationStateCreateInfo m_rasterizerStage{};
  VkPipelineMultisampleStateCreateInfo m_multisampleStage{};


  VkPipelineLayout m_layout{};

  VkViewport m_viewport{};
  VkRect2D m_scissors{};
  U32 m_currentLocation{0};

  const Log log_VulkanRenderSystem;
};

}  // namespace Vulkan
}  // namespace Azura
