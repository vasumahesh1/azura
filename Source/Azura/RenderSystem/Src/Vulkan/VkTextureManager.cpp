#include <utility>
#include "Vulkan/VkTextureManager.h"

namespace Azura {

VkTextureManager::VkTextureManager(VkDevice device, U32 poolSize, U32 maxTextures, Log log)
  : TextureManager(poolSize, maxTextures),
    m_device(device),
    log_VulkanRenderSystem(std::move(log)) {
}
} // namespace Azura
