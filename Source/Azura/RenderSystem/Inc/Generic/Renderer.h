#pragma once

#include "Constants.h"
#include "Containers/Vector.h"
#include "Drawable.h"
#include "GenericTypes.h"
#include "Core/RawStorageFormat.h"

namespace Azura {
const U32 DEFAULT_FRAMES_IN_FLIGHT = 2;

struct DrawablePoolCreateInfo;

struct DeviceRequirements {
  bool m_discreteGPU{true};
  bool m_transferQueue{false};
  bool m_float64{false};
  bool m_int64{false};
};

struct ApplicationInfo {
  std::string m_name;
  Version m_version;
};

struct ApplicationRequirements {
  float m_clearColor[4] {0, 0, 0, 1.0f};
  float m_depthStencilClear[2] {1.0f, 0};
};

struct SwapChainRequirements {
  RawStorageFormat m_format{};
  RawStorageFormat m_depthFormat{};
  Bounds2D m_extent{0u, 0u};
  ColorSpace m_colorSpace{};
  U32 m_framesInFlight{DEFAULT_FRAMES_IN_FLIGHT};
};

struct ShaderCreateInfo
{
  U32 m_id;
  ShaderStage m_stage;
  String m_shaderFileName;
  AssetLocation location;
};

struct RenderPassRequirements
{
  Containers::Vector<RenderPassBufferCreateInfo> m_renderPassBuffers;
  Containers::Vector<PipelinePassCreateInfo> m_renderPassSequence;
  Containers::Vector<ShaderCreateInfo> m_shaders;

  explicit RenderPassRequirements(Memory::Allocator& alloc);
};

struct DescriptorRequirements
{
  Containers::Vector<DescriptorSlotCreateInfo> m_descriptorSlots;

  explicit DescriptorRequirements(Memory::Allocator& alloc);
};

class Renderer {
public:
  Renderer(ApplicationInfo appInfo, const DeviceRequirements& deviceRequirements,
           ApplicationRequirements appRequirements, const SwapChainRequirements& swapChainRequirements, Memory::Allocator& allocator);
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
