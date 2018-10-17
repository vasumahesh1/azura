#pragma once
#include "Generic/Renderer.h"
#include "Generic/Window.h"


namespace Azura {
namespace D3D12 {

class D3D12Renderer : public Renderer {

public:
  D3D12Renderer(const ApplicationInfo& appInfo,
                const DeviceRequirements& deviceRequirements,
                const ApplicationRequirements& appRequirements,
                const SwapChainRequirements& swapChainRequirements,
                const RenderPassRequirements& renderPassRequirements,
                const DescriptorRequirements& descriptorRequirements,
                const ShaderRequirements& shaderRequirements,
                Memory::Allocator& mainAllocator,
                Memory::Allocator& drawAllocator,
                Window& window);

  String GetRenderingAPI() const override;
  DrawablePool& CreateDrawablePool(const DrawablePoolCreateInfo& createInfo) override;
  void Submit() override;
  void RenderFrame() override;
  void SnapshotFrame(const String& exportPath) const override;

private:
  void AddShader(const ShaderCreateInfo& info) override;
};

} // namespace D3D12
} // namespace Azura
