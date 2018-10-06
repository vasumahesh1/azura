#pragma once
#include "Generic/TextureManager.h"
#include <vulkan/vulkan_core.h>

namespace Azura {

class VkTextureManager : public TextureManager
{
public:
  VkTextureManager(VkDevice device, U32 poolSize, U32 maxTextures, Log log);

private:
  VkDevice m_device;

  Log log_VulkanRenderSystem;
};

} // namespace Azura