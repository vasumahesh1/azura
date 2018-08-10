#include "Vulkan/VkRenderer.h"

namespace Azura {
namespace Vulkan {

VulkanRenderer::VulkanRenderer(const ApplicationInfo& appInfo, const DeviceRequirements &deviceRequirements) : Renderer(appInfo, deviceRequirements)
{
}

VulkanRenderer::~VulkanRenderer() = default;

void VulkanRenderer::AddRenderEntity() {
}

} // namespace Vulkan
} // namespace Azura
