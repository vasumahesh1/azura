#include "Vulkan/VkShader.h"
#include "Utils/FileReader.h"
#include "Vulkan/VkCore.h"
#include "Vulkan/VkMacros.h"
#include "Vulkan/VkTypeMapping.h"
#include "Memory/MemoryFactory.h"
#include "Memory/MonotonicAllocator.h"
#include "Memory/HeapMemoryBuffer.h"
#include <iostream>

namespace Azura {
namespace Vulkan {

namespace {
const String SPRIV_EXT = "spv";
} // namespace

VkShader::VkShader(VkDevice device, const String& fileName, const Log& logger) // NOLINT
  : Shader(fileName, SPRIV_EXT),
    log_VulkanRenderSystem(logger) {
  HEAP_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 8192);

  const auto fileContents = FileReader::GetFileContents(GetFilePath(), allocatorTemporary);
  m_module = VkCore::CreateShaderModule(device, fileContents, log_VulkanRenderSystem);
}

VkPipelineShaderStageCreateInfo VkShader::GetShaderStageInfo() const {
  const auto vkShaderStage = ToVkShaderStageFlagBits(GetShaderStage());
  VERIFY_OPT(log_VulkanRenderSystem, vkShaderStage, "Unknown Shader Stage");

  VkPipelineShaderStageCreateInfo shaderStage = {};
  shaderStage.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStage.stage                           = vkShaderStage.value();
  shaderStage.module                          = m_module;
  shaderStage.pName                           = GetShaderEntryPoint().c_str();

  return shaderStage;
}

void VkShader::CleanUp(VkDevice device) const {
  vkDestroyShaderModule(device, m_module, nullptr);
}

} // namespace Vulkan
} // namespace Azura
