#include "Vulkan/VkScopedPipeline.h"
#include "Vulkan/VkTypeMapping.h"
#include "Vulkan/VkMacros.h"
#include "Memory/Allocator.h"

namespace Azura {
namespace Vulkan {

VkScopedPipeline::VkScopedPipeline(VkPipeline pipeline)
  : m_pipeline(pipeline) {
}

VkPipeline VkScopedPipeline::Real() const {
  return m_pipeline;
}

// TODO(vasumahesh1): Figure out a way to adjust size properly
VkPipelineFactory::VkPipelineFactory(VkDevice device, Memory::Allocator& allocator)
  : m_device(device),
  m_stages(10, allocator),
  m_bindingInfo(10, allocator),
  m_attributeDescription(10, allocator){
}

VkPipelineFactory& VkPipelineFactory::AddShaderStage(VkPipelineShaderStageCreateInfo shaderStageCreateInfo) {
  // TODO(vasumahesh1): Emplace?
  m_stages.PushBack(shaderStageCreateInfo);
  return *this;
}

VkPipelineFactory& VkPipelineFactory::AddBindingDescription(U32 stride, Slot slot) {
  VkVertexInputBindingDescription bindingDesc;

  const auto rate = ToVkVertexInputRate(slot.m_rate);
  VERIFY_OPT(rate, "Unknown Format");

  bindingDesc.binding = slot.m_binding;
  bindingDesc.stride = stride;
  bindingDesc.inputRate = rate.value();

  m_bindingInfo.PushBack(bindingDesc);
  return *this;
}

VkPipelineFactory& VkPipelineFactory::AddAttributeDescription(RawStorageFormat rawFormat, U32 binding) {
  auto bindingInfo = m_bindingMap[binding];

  const auto format = ToVkFormat(rawFormat);
  VERIFY_OPT(format, "Unknown Format");

  VkVertexInputAttributeDescription attrDesc;
  attrDesc.binding  = binding;
  attrDesc.location = bindingInfo.m_location;
  attrDesc.format   = format.value();
  attrDesc.offset   = bindingInfo.m_offset;

  // TODO(vasumahesh1): Handle 64bit formats taking 2 locations
  bindingInfo.m_location++;

  bindingInfo.m_offset += GetFormatSize(rawFormat);

  m_bindingMap[binding] = bindingInfo;

  m_attributeDescription.PushBack(attrDesc);
  return *this;
}

VkPipelineFactory& VkPipelineFactory::BulkAddAttributeDescription(const Containers::Vector<RawStorageFormat>& strides,
  U32 binding) {

  for (const auto& rawFormat : strides) {

    auto bindingInfo = m_bindingMap[binding];

    const auto format = ToVkFormat(rawFormat);
    VERIFY_OPT(format, "Unknown Format");

    VkVertexInputAttributeDescription attrDesc;
    attrDesc.binding = binding;
    attrDesc.location = bindingInfo.m_location;
    attrDesc.format = format.value();
    attrDesc.offset = bindingInfo.m_offset;

    // TODO(vasumahesh1): Handle 64bit formats taking 2 locations
    bindingInfo.m_location++;

    bindingInfo.m_offset += GetFormatSize(rawFormat);

    m_bindingMap[binding] = bindingInfo;

    m_attributeDescription.PushBack(attrDesc);
  }

  return *this;
}

VkPipelineFactory& VkPipelineFactory::SetInputAssemblyStage(PrimitiveTopology topology) {
  const auto vkTopology = ToVkPrimitiveTopology(topology);
  VERIFY_OPT(vkTopology, "Unknown Topology");

  m_inputAssemblyStage.sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  m_inputAssemblyStage.topology = vkTopology.value();

  // TODO(vasumahesh1): Might need to expose
  m_inputAssemblyStage.primitiveRestartEnable = VK_FALSE;

  return *this;
}


VkPipelineFactory& VkPipelineFactory::SetViewportStage(ViewportDimensions viewportDimensions,
                                                       const VkScopedSwapChain& swapChain) {
  VkViewport viewport;
  viewport.x        = viewportDimensions.m_x;
  viewport.y        = viewportDimensions.m_y;
  viewport.width    = viewportDimensions.m_width;
  viewport.height   = viewportDimensions.m_height;
  viewport.minDepth = viewportDimensions.m_minDepth;
  viewport.maxDepth = viewportDimensions.m_maxDepth;

  // TODO(vasumahesh1): Might need custom scissoring
  VkRect2D scissor;
  scissor.offset   = {0, 0};
  scissor.extent   = swapChain.m_extent;

  // TODO(vasumahesh1): Might need arrays here one day
  m_viewportStage.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  m_viewportStage.viewportCount = 1;
  m_viewportStage.pViewports    = &viewport;
  m_viewportStage.scissorCount  = 1;
  m_viewportStage.pScissors     = &scissor;

  return *this;
}

VkPipelineFactory& VkPipelineFactory::SetRasterizerStage(CullMode cullMode, FrontFace faceOrder) {
  const auto vkCullMode = ToVkCullModeFlags(cullMode);
  VERIFY_OPT(vkCullMode, "Unknown Cull Mode");

  const auto vkFrontFace = ToVkFrontFace(faceOrder);
  VERIFY_OPT(vkFrontFace, "Unknown Face Order");

  // TODO(vasumahesh1): Might need to expose some values later on
  m_rasterizerStage.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  m_rasterizerStage.depthClampEnable        = VK_FALSE;
  m_rasterizerStage.rasterizerDiscardEnable = VK_FALSE;
  m_rasterizerStage.polygonMode             = VK_POLYGON_MODE_FILL;
  m_rasterizerStage.lineWidth               = 1.0f;
  m_rasterizerStage.cullMode                = vkCullMode.value();
  m_rasterizerStage.frontFace               = vkFrontFace.value();
  m_rasterizerStage.depthBiasEnable         = VK_FALSE;

  return *this;
}

// Not Supported Yet
VkPipelineFactory& VkPipelineFactory::SetMultisampleStage() {
  m_multisampleStage.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  m_multisampleStage.sampleShadingEnable  = VK_FALSE;
  m_multisampleStage.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  // TODO(vasumahesh1): Add Support

  return *this;
}


VkPipelineFactory& VkPipelineFactory::SetColorBlendStage() {
  VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
  colorBlendAttachment.colorWriteMask                      =
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
    VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;

  // TODO(vasumahesh1): Add Support for Blending
  m_colorBlendStage.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  m_colorBlendStage.logicOpEnable   = VK_FALSE;
  m_colorBlendStage.attachmentCount = 1;
  m_colorBlendStage.pAttachments    = &colorBlendAttachment;

  return *this;
}

VkPipelineFactory& VkPipelineFactory::SetPipelineLayout(VkPipelineLayout layout) {
  m_layout = layout;
  return *this;
}

// TODO(vasumahesh1): Fix this, find a generic way to make a render pass
VkPipelineFactory& VkPipelineFactory::SetRenderPass(VkRenderPass renderPass) {
  m_renderPass = renderPass;
  return *this;
}

VkScopedPipeline VkPipelineFactory::Submit() const {
  VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
  vertexInputInfo.sType                                = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount        = m_bindingInfo.GetSize();
  vertexInputInfo.pVertexBindingDescriptions           = m_bindingInfo.Data();
  vertexInputInfo.vertexAttributeDescriptionCount      = m_attributeDescription.GetSize();
  vertexInputInfo.pVertexAttributeDescriptions         = m_attributeDescription.Data();

  VkGraphicsPipelineCreateInfo pipelineInfo = {};
  pipelineInfo.sType                        = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount                   = m_stages.GetSize();
  pipelineInfo.pStages                      = m_stages.Data();
  pipelineInfo.pVertexInputState            = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState          = &m_inputAssemblyStage;
  pipelineInfo.pViewportState               = &m_viewportStage;
  pipelineInfo.pRasterizationState          = &m_rasterizerStage;
  pipelineInfo.pMultisampleState            = &m_multisampleStage;
  pipelineInfo.pDepthStencilState           = nullptr;
  pipelineInfo.pColorBlendState             = &m_colorBlendStage;
  pipelineInfo.pDynamicState                = nullptr;
  pipelineInfo.layout                       = m_layout;
  pipelineInfo.renderPass                   = m_renderPass;
  pipelineInfo.subpass                      = 0;
  pipelineInfo.basePipelineHandle           = VK_NULL_HANDLE;
  pipelineInfo.basePipelineIndex            = -1;

  VkPipeline pipeline;
  VERIFY_VK_OP(vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline), "Failed to create pipeline");
  return VkScopedPipeline(pipeline);
}
} // namespace Vulkan
} // namespace Azura
