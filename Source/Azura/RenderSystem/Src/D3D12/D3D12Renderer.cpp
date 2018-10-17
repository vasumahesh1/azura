#include "D3D12/D3D12Renderer.h"


namespace Azura {
namespace D3D12 {

D3D12Renderer::D3D12Renderer(const ApplicationInfo& appInfo,
                             const DeviceRequirements& deviceRequirements,
                             const ApplicationRequirements& appRequirements,
                             const SwapChainRequirements& swapChainRequirements,
                             const RenderPassRequirements& renderPassRequirements,
                             const DescriptorRequirements& descriptorRequirements,
                             const ShaderRequirements& shaderRequirements,
                             Memory::Allocator& mainAllocator,
                             Memory::Allocator& drawAllocator,
                             Window& window)
  : Renderer(appInfo, deviceRequirements, appRequirements, swapChainRequirements, descriptorRequirements,
             mainAllocator, drawAllocator, window) {
  UNUSED(renderPassRequirements);
  UNUSED(shaderRequirements);
}

String D3D12Renderer::GetRenderingAPI() const {
  return "D3D12";
}

DrawablePool& D3D12Renderer::CreateDrawablePool(const DrawablePoolCreateInfo& createInfo) {
}

void D3D12Renderer::Submit() {
}

void D3D12Renderer::RenderFrame() {
}

void D3D12Renderer::SnapshotFrame(const String& exportPath) const {
}

void D3D12Renderer::AddShader(const ShaderCreateInfo& info) {
}
} // namespace D3D12
} // namespace Azura
