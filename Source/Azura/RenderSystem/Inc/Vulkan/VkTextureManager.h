#pragma once
#include "Generic/TextureManager.h"
#include <vulkan/vulkan_core.h>
#include "Generic/Renderer.h"

namespace Azura {
namespace Vulkan {

class VkTextureManager final : public TextureManager
{
public:
  VkTextureManager(const TextureRequirements& requirements);

private:
};

} // namespace Vulkan
} // namespace Azura