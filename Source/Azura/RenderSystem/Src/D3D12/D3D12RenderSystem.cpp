#include "Generic/RenderSystem.h"
#include "D3D12/D3D12Renderer.h"
#include "D3D12/D3D12TextureManager.h"

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
  return std::make_unique<D3D12::D3D12Renderer>(appInfo, deviceRequirements, appRequirements, swapChainRequirement,
                                                renderPassRequirements, descriptorRequirements, shaderRequirements,
                                                mainAllocator,
                                                drawAllocator, window);
}

std::unique_ptr<TextureManager> RenderSystem::CreateTextureManager(const TextureRequirements& textureRequirements) {
  return std::make_unique<D3D12::D3D12TextureManager>(textureRequirements);
}

} // namespace Azura
