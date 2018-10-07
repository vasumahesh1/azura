#include <utility>
#include "Vulkan/VkTextureManager.h"

namespace Azura {
namespace Vulkan {

VkTextureManager::VkTextureManager(VkDevice device, const TextureRequirements& requirements, Log log)
  : TextureManager(requirements),
    m_device(device),
    log_VulkanRenderSystem(std::move(log)) {
}
} // namespace Vulkan
} // namespace Azura
