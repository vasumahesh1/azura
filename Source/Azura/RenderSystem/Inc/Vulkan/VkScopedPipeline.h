#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>

#include "Generic/GenericTypes.h"
#include "Vulkan/VkTypes.h"

namespace Azura {
namespace Vulkan {

class VkScopedPipeline {

public:
  VkScopedPipeline(VkPipeline pipeline);
  VkPipeline Real() const;

private:
  VkPipeline m_pipeline;
};

class VkPipelineFactory {

public:
  VkPipelineFactory(VkDevice device);

  VkPipelineFactory& AddShaderStage(VkPipelineShaderStageCreateInfo shaderStageCreateInfo);

  VkPipelineFactory& AddBindingDescription(VkVertexInputBindingDescription bindingDesc);
  VkPipelineFactory& AddAttributeDescription(VkVertexInputAttributeDescription attrDesc);

  VkPipelineFactory& BulkAddAttributeDescription(std::vector<VkVertexInputAttributeDescription>&& attributes);
  VkPipelineFactory& SetInputAssemblyStage(PrimitiveTopology topology);
  VkPipelineFactory& SetViewportStage(ViewportDimensions viewportDimensions, const VkScopedSwapChain& swapChain);
  VkPipelineFactory& SetRasterizerStage(CullMode cullMode, FrontFace faceOrder);
  VkPipelineFactory& SetMultisampleStage();
  VkPipelineFactory& SetColorBlendStage();
  VkPipelineFactory& SetPipelineLayout(VkPipelineLayout layout);
  VkPipelineFactory& SetRenderPass(VkRenderPass renderPass);

  VkScopedPipeline Submit() const;

private:
  const VkDevice m_device;

  // TODO(vasumahesh1): use Azura Vector
  std::vector<VkPipelineShaderStageCreateInfo> m_stages;
  std::vector<VkVertexInputBindingDescription> m_bindingInfo;
  std::vector<VkVertexInputAttributeDescription> m_attributeDescription;

  VkPipelineInputAssemblyStateCreateInfo m_inputAssemblyStage{};
  VkPipelineViewportStateCreateInfo m_viewportStage{};
  VkPipelineRasterizationStateCreateInfo m_rasterizerStage{};
  VkPipelineMultisampleStateCreateInfo m_multisampleStage{};
  VkPipelineColorBlendStateCreateInfo m_colorBlendStage{};

  VkPipelineLayout m_layout{};
  VkRenderPass m_renderPass{};
};

} // namespace Vulkan
} // namespace Azura
