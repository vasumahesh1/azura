#include "Generic/RenderSystem.h"
#include "Generic/Window.h"
#include "Vulkan/VkRenderer.h"
#include "Vulkan/VkShader.h"
#include "Vulkan/VkTextureManager.h"

namespace Azura {
std::unique_ptr<Renderer> RenderSystem::CreateRenderer(const ApplicationInfo& appInfo,
                                                       const DeviceRequirements& deviceRequirements,
                                                       const ApplicationRequirements& appRequirements,
                                                       const SwapChainRequirements& swapChainRequirement,
                                                       const RenderPassRequirements& renderPassRequirements,
                                                       const DescriptorRequirements& descriptorRequirements,
                                                       Memory::Allocator& mainAllocator,
                                                       Memory::Allocator& drawAllocator,
                                                       Window& window) {
  return std::make_unique<Vulkan::VkRenderer>(appInfo, deviceRequirements, appRequirements, swapChainRequirement, renderPassRequirements, descriptorRequirements, mainAllocator,
                                              drawAllocator, window);
}

std::unique_ptr<TextureManager> RenderSystem::CreateTextureManager(const Renderer& renderer, const TextureRequirements& textureRequirements, const Log& logger)
{
  const auto& vkRenderer = reinterpret_cast<const Vulkan::VkRenderer&>(renderer); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
  return std::make_unique<Vulkan::VkTextureManager>(vkRenderer.GetDevice(), textureRequirements, logger);
}

}  // namespace Azura
