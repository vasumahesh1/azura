#include "Generic/RenderSystem.h"
#include "Generic/Window.h"
#include "Vulkan/VkRenderer.h"

namespace Azura {
std::unique_ptr<Renderer> RenderSystem::CreateRenderer(const ApplicationInfo& appInfo,
                                                       const DeviceRequirements& deviceRequirements,
                                                       const SwapChainRequirement& swapChainRequirement,
                                                       Memory::Allocator& mainAllocator,
                                                       Memory::Allocator& drawAllocator,
                                                       Window& window) {
  return std::make_unique<Vulkan::VkRenderer>(appInfo, deviceRequirements, swapChainRequirement, mainAllocator,
                                              drawAllocator, window);
}
}  // namespace Azura
