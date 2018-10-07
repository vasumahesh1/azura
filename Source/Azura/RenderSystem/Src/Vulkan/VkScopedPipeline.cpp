#include "Vulkan/VkScopedPipeline.h"
#include "Memory/Allocator.h"
#include "Vulkan/VkMacros.h"
#include "Vulkan/VkTypeMapping.h"

namespace Azura {
namespace Vulkan {

VkScopedPipeline::VkScopedPipeline(VkPipeline pipeline)
  : m_pipeline(pipeline) {
}

VkPipeline VkScopedPipeline::Real() const {
  return m_pipeline;
}

void VkScopedPipeline::CleanUp(VkDevice device) const {
  vkDestroyPipeline(device, m_pipeline, nullptr);
}

// TODO(vasumahesh1): Figure out a way to adjust size properly
VkPipelineFactory::VkPipelineFactory(VkDevice device, Memory::Allocator& allocator, Log logger)
  : m_device(device),
    m_stages(10, allocator),
    m_bindingInfo(10, allocator),
    m_attributeDescription(10, allocator),
    m_colorBlendAttachment(),
    log_VulkanRenderSystem(std::move(logger)) {
}

VkPipelineFactory& VkPipelineFactory::AddShaderStage(const VkPipelineShaderStageCreateInfo& shaderStageCreateInfo) {
  // TODO(vasumahesh1): Emplace?
  m_stages.PushBack(shaderStageCreateInfo);
  return *this;
}

VkPipelineFactory& VkPipelineFactory::AddBindingDescription(U32 stride, VertexSlot slot, U32 binding) {
  VkVertexInputBindingDescription bindingDesc;

  const auto rate = ToVkVertexInputRate(slot.m_rate);
  VERIFY_OPT(log_VulkanRenderSystem, rate, "Unknown Format");

  bindingDesc.binding   = binding;
  bindingDesc.stride    = stride;
  bindingDesc.inputRate = rate.value();

  m_bindingInfo.PushBack(bindingDesc);
  return *this;
}

VkPipelineFactory& VkPipelineFactory::AddAttributeDescription(RawStorageFormat rawFormat, U32 binding) {
  auto bindingInfo = m_bindingMap[binding];

  const auto format = ToVkFormat(rawFormat);
  VERIFY_OPT(log_VulkanRenderSystem, format, "Unknown Format");

  VkVertexInputAttributeDescription attrDesc;
  attrDesc.binding  = binding;
  attrDesc.location = m_currentLocation;
  attrDesc.format   = format.value();
  attrDesc.offset   = bindingInfo.m_offset;

  // TODO(vasumahesh1):[FORMATS]: Handle 64bit formats taking 2 locations
  m_currentLocation++;

  bindingInfo.m_offset += GetFormatSizeBits(rawFormat);

  m_bindingMap[binding] = bindingInfo;

  m_attributeDescription.PushBack(attrDesc);
  return *this;
}

VkPipelineFactory& VkPipelineFactory::BulkAddAttributeDescription(const Containers::Vector<RawStorageFormat>& strides,
                                                                  U32 binding) {
  auto bindingInfo = m_bindingMap[binding];

  for (const auto& rawFormat : strides) {
    const auto format = ToVkFormat(rawFormat);
    VERIFY_OPT(log_VulkanRenderSystem, format, "Unknown Format");

    VkVertexInputAttributeDescription attrDesc;
    attrDesc.binding  = binding;
    attrDesc.location = m_currentLocation;
    attrDesc.format   = format.value();
    attrDesc.offset   = bindingInfo.m_offset;

    // TODO(vasumahesh1):[FORMATS]: Handle 64bit formats taking 2 locations
    m_currentLocation++;

    bindingInfo.m_offset += GetFormatSize(rawFormat);

    m_attributeDescription.PushBack(attrDesc);
  }

  m_bindingMap[binding] = bindingInfo;

  return *this;
}

VkPipelineFactory& VkPipelineFactory::SetInputAssemblyStage(PrimitiveTopology topology) {
  const auto vkTopology = ToVkPrimitiveTopology(topology);
  VERIFY_OPT(log_VulkanRenderSystem, vkTopology, "Unknown Topology");

  m_inputAssemblyStage.sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  m_inputAssemblyStage.topology = vkTopology.value();

  // TODO(vasumahesh1): Might need to expose
  m_inputAssemblyStage.primitiveRestartEnable = VK_FALSE;

  return *this;
}

VkPipelineFactory& VkPipelineFactory::SetViewportStage(ViewportDimensions viewportDimensions,
                                                       const VkScopedSwapChain& swapChain) {
  m_viewport.x        = viewportDimensions.m_x;
  m_viewport.y        = viewportDimensions.m_y;
  m_viewport.width    = viewportDimensions.m_width;
  m_viewport.height   = viewportDimensions.m_height;
  m_viewport.minDepth = viewportDimensions.m_minDepth;
  m_viewport.maxDepth = viewportDimensions.m_maxDepth;

  // TODO(vasumahesh1): Might need custom scissoring
  m_scissors.offset = {0, 0};
  m_scissors.extent = swapChain.GetExtent();

  // TODO(vasumahesh1): Might need arrays here one day
  m_viewportStage.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  m_viewportStage.viewportCount = 1;
  m_viewportStage.pViewports    = &m_viewport;
  m_viewportStage.scissorCount  = 1;
  m_viewportStage.pScissors     = &m_scissors;

  return *this;
}

VkPipelineFactory& VkPipelineFactory::SetRasterizerStage(CullMode cullMode, FrontFace faceOrder) {
  const auto vkCullMode = ToVkCullModeFlags(cullMode);
  VERIFY_OPT(log_VulkanRenderSystem, vkCullMode, "Unknown Cull Mode");

  const auto vkFrontFace = ToVkFrontFace(faceOrder);
  VERIFY_OPT(log_VulkanRenderSystem, vkFrontFace, "Unknown Face Order");

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
  m_colorBlendAttachment                = {};
  m_colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
                                          | VK_COLOR_COMPONENT_A_BIT;
  m_colorBlendAttachment.blendEnable = VK_FALSE;

  // TODO(vasumahesh1): Add Support for Blending
  m_colorBlendStage.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  m_colorBlendStage.logicOpEnable   = VK_FALSE;
  m_colorBlendStage.attachmentCount = 1;
  m_colorBlendStage.pAttachments    = &m_colorBlendAttachment;

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

  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Creating Pipeline");
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Num Attribute Descriptions: %d", m_attributeDescription.GetSize());
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Num Binding Info: %d", m_bindingInfo.GetSize());
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Total Shader Stages: %d", m_stages.GetSize());

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
  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline),
    "Failed to create pipeline");
  return VkScopedPipeline(pipeline);
}
} // namespace Vulkan
} // namespace Azura
