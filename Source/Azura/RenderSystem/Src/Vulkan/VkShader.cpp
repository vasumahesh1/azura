#include "Vulkan/VkShader.h"
#include "Vulkan/VkCore.h"
#include "Utils/FileReader.h"
#include "Vulkan/VkMacros.h"
#include "Vulkan/VkTypeMapping.h"

namespace Azura {
namespace Vulkan {

namespace {
const String SPRIV_EXT = "spv";
} // namespace

VkShader::VkShader(VkDevice device, const String& fileName)
  : Shader(fileName, SPRIV_EXT) {

  const auto fileContents = FileReader::GetFileContents(GetFilePath());
  m_module                = VkCore::CreateShaderModule(device, fileContents);
}

VkPipelineShaderStageCreateInfo VkShader::GetShaderStageInfo() const {
  const auto vkShaderStage = ToVkShaderStageFlagBits(GetShaderStage());
  VERIFY_OPT(vkShaderStage, "Unknown Shader Stage");

  VkPipelineShaderStageCreateInfo shaderStage = {};
  shaderStage.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStage.stage                           = vkShaderStage.value();
  shaderStage.module                          = m_module;
  shaderStage.pName                           = GetShaderEntryPoint().c_str();

  return shaderStage;
}

const std::vector<VkVertexInputAttributeDescription>& VkShader::GetInputAttributeDescription() const {
  return m_attributes;
}

void VkShader::AddVertexAttribute(VkFormat format, U32 binding) {
  auto bindingInfo = m_bindingMap[binding];

  VkVertexInputAttributeDescription attrDesc;
  attrDesc.binding  = binding;
  attrDesc.location = bindingInfo.m_location;
  attrDesc.format   = format;
  attrDesc.offset   = bindingInfo.m_offset;

  // TODO(vasumahesh1): Handle 64bit formats taking 2 locations
  bindingInfo.m_location++;

  // TODO(vasumahesh1): Handle SIZE in Offset
  bindingInfo.m_offset += 0;

  m_bindingMap[binding] = bindingInfo;

  m_attributes.push_back(attrDesc);
}
} // namespace Vulkan
} // namespace Azura
