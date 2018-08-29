#pragma once

#include "Generic/Shader.h"
#include <vulkan/vulkan_core.h>
#include <map>

namespace Azura {
namespace Memory {
class Allocator;
}

enum class RawStorageFormat;
}

namespace Azura {
namespace Vulkan {

class VkShader : public Shader {
public:
  VkShader(VkDevice device, const String& fileName, Memory::Allocator& temporaryAllocator);
  VkPipelineShaderStageCreateInfo GetShaderStageInfo() const;

private:
  VkShaderModule m_module{};
};


} // namespace Vulkan
} // namespace Azura
