#pragma once

#include <vulkan/vulkan_core.h>
#include <map>
#include "Generic/Shader.h"
#include "Log/Log.h"

namespace Azura {
enum class RawStorageFormat;
}  // namespace Azura

namespace Azura {
namespace Vulkan {

class VkShader : public Shader {
 public:
  VkShader(U32 id, VkDevice device, const String& fileName, const Log& logger);
  VkPipelineShaderStageCreateInfo GetShaderStageInfo() const;

  void CleanUp(VkDevice device) const;

 private:
  VkShaderModule m_module{};
  const Log log_VulkanRenderSystem;
};

}  // namespace Vulkan
}  // namespace Azura
