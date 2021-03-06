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
                                                       const ShaderRequirements& shaderRequirements,
                                                       Memory::Allocator& mainAllocator,
                                                       Memory::Allocator& drawAllocator,
                                                       Window& window) {
  return std::make_unique<Vulkan::VkRenderer>(appInfo, deviceRequirements, appRequirements, swapChainRequirement,
                                              renderPassRequirements, descriptorRequirements, shaderRequirements, mainAllocator,
                                              drawAllocator, window);
}

std::unique_ptr<TextureManager> RenderSystem::CreateTextureManager(const TextureRequirements& textureRequirements) {
  return std::make_unique<Vulkan::VkTextureManager>(textureRequirements);
}

} // namespace Azura
