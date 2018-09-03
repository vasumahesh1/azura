#include "Generic/RenderSystem.h"
#include "Generic/Window.h"
#include "Vulkan/VkRenderer.h"
#include "Vulkan/VkShader.h"

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

std::unique_ptr<Shader> RenderSystem::CreateShader(const Renderer& renderer, const String& fileName) {
  const auto& vkRenderer = reinterpret_cast<const Vulkan::VkRenderer&>(renderer); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
  return std::make_unique<Vulkan::VkShader>(vkRenderer.GetDevice(), fileName);
}
}  // namespace Azura
