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

VkShader::VkShader(VkDevice device, const String& fileName, Memory::Allocator& allocator)
  : Shader(fileName, SPRIV_EXT),
  m_attributes(allocator) {

  const auto fileContents = FileReader::GetFileContents(GetFilePath(), allocator);
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

const Containers::Vector<VkVertexInputAttributeDescription>& VkShader::GetInputAttributeDescription() const {
  return m_attributes;
}

void VkShader::SetVertexInputAttributeCount(U32 count) {
  m_attributes.Reserve(count);
}

void VkShader::AddVertexAttribute(RawStorageFormat rawFormat, U32 binding) {
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

  m_attributes.PushBack(attrDesc);
}
} // namespace Vulkan
} // namespace Azura
