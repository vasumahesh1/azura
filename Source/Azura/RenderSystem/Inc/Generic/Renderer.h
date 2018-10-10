#pragma once

#include "Constants.h"
#include "Containers/Vector.h"
#include "Drawable.h"
#include "GenericTypes.h"
#include "Core/RawStorageFormat.h"

namespace Azura {

struct DrawablePoolCreateInfo;

class Renderer {
public:
  Renderer(ApplicationInfo appInfo,
           const DeviceRequirements& deviceRequirements,
           ApplicationRequirements appRequirements,
           const SwapChainRequirements& swapChainRequirements,
           const DescriptorRequirements& descriptorRequirements,
           Memory::Allocator& allocator);
  virtual ~Renderer() = default;

  Renderer(const Renderer& other) = delete;
  Renderer(Renderer&& other) = delete;
  Renderer& operator=(const Renderer& rhs) = delete;
  Renderer& operator=(Renderer&& rhs) = delete;

  virtual String GetRenderingAPI() const = 0;

  virtual void SetDrawablePoolCount(U32 count) = 0;
  virtual DrawablePool& CreateDrawablePool(const DrawablePoolCreateInfo& createInfo) = 0;

  virtual void Submit() = 0;
  virtual void RenderFrame() = 0;

  virtual void EnterRenderFrame();
  virtual void ExitRenderFrame();

  virtual void SnapshotFrame(const String& exportPath) const = 0;

protected:
  const ApplicationInfo& GetApplicationInfo() const;
  const DeviceRequirements& GetDeviceRequirements() const;
  const ApplicationRequirements& GetApplicationRequirements() const;
  const SwapChainRequirements& GetSwapchainRequirements() const;
  Memory::Allocator& GetAllocator() const;

  U32 GetCurrentFrame() const;

  Containers::Vector<DescriptorSlot> m_descriptorSlots;

  DescriptorCount m_descriptorCount;

private:
  virtual void AddShader(const ShaderCreateInfo& info) = 0;

  const ApplicationInfo m_applicationInfo;
  const DeviceRequirements m_deviceRequirements;
  const ApplicationRequirements m_appRequirements;
  SwapChainRequirements m_swapChainRequirements;
  Memory::Allocator& m_allocator;

  U32 m_frameCount;
};
} // namespace Azura
