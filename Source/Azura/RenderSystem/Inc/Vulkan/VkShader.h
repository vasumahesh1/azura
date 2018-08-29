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
  VkShader(VkDevice device, const String& fileName, Memory::Allocator& allocator);

  VkPipelineShaderStageCreateInfo GetShaderStageInfo() const;

  const Containers::Vector<VkVertexInputAttributeDescription>& GetInputAttributeDescription() const;

  void SetVertexInputAttributeCount(U32 count);
  void AddVertexAttribute(RawStorageFormat rawFormat, U32 binding);

private:
  struct BindingInfo {
    U32 m_offset{0};
    U32 m_location{0};
  };

  // TODO(vasumahesh1): Make our own map
  std::map<U32, BindingInfo> m_bindingMap;

  VkShaderModule m_module{};
  Containers::Vector<VkVertexInputAttributeDescription> m_attributes;
};


} // namespace Vulkan
} // namespace Azura
