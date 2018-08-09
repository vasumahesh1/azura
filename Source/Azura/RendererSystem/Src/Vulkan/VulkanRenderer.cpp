#include "Vulkan/VulkanRenderer.h"

namespace Azura {
VulkanRenderer::VulkanRenderer(const ApplicationInfo& appInfo, const DeviceRequirements &deviceRequirements) : Renderer(appInfo, deviceRequirements)
{
}

VulkanRenderer::~VulkanRenderer() {
}

void VulkanRenderer::AddRenderEntity() {
}

} // namespace Azura
