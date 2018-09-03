#pragma once

#include <vulkan/vulkan_core.h>
#include <map>
#include "Generic/Shader.h"

namespace Azura {
enum class RawStorageFormat;
}  // namespace Azura

namespace Azura {
namespace Vulkan {

class VkShader : public Shader {
 public:
  VkShader(VkDevice device, const String& fileName);
  VkPipelineShaderStageCreateInfo GetShaderStageInfo() const;

 private:
  VkShaderModule m_module{};
};

}  // namespace Vulkan
}  // namespace Azura
