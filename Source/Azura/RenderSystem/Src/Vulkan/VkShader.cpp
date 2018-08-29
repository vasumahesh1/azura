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

VkShader::VkShader(VkDevice device, const String& fileName, Memory::Allocator& temporaryAllocator)
  : Shader(fileName, SPRIV_EXT) {

  const auto fileContents = FileReader::GetFileContents(GetFilePath(), temporaryAllocator);
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

} // namespace Vulkan
} // namespace Azura
