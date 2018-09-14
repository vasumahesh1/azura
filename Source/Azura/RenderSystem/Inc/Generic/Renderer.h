#pragma once

#include "Constants.h"
#include "Containers/Vector.h"
#include "Drawable.h"
#include "GenericTypes.h"
#include "RawStorageFormat.h"

namespace Azura {
const U32 DEFAULT_FRAMES_IN_FLIGHT = 2;

struct DrawablePoolCreateInfo;

struct Bounds3D {
  U32 m_width{0};
  U32 m_height{0};
  U32 m_depth{0};

  Bounds3D(U32 width, U32 height, U32 depth)
    : m_width(width),
      m_height(height),
      m_depth(depth) {

  }
};

struct Bounds2D {
  U32 m_width{0};
  U32 m_height{0};

  Bounds2D(U32 width, U32 height)
    : m_width(width),
      m_height(height) {

  }
};

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

struct UniformBufferDesc
{
  U32 m_size;
  U32 m_count;
  U32 m_binding;
};

struct ApplicationRequirements {
  Containers::Vector<std::pair<ShaderStage, UniformBufferDesc>> m_uniformBuffers;
  float m_clearColor[4] {0, 0, 0, 1.0f};

  ApplicationRequirements(Memory::Allocator& alloc) : m_uniformBuffers(alloc)
  {
  }
};

struct SwapChainRequirements {
  RawStorageFormat m_format{};
  Bounds2D m_extent{0u, 0u};
  ColorSpace m_colorSpace{};
  U32 m_framesInFlight{DEFAULT_FRAMES_IN_FLIGHT};
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
  Memory::Allocator& GetAllocator() const;

  U32 GetCurrentFrame() const;

private:
  const ApplicationInfo m_applicationInfo;
  const DeviceRequirements m_deviceRequirements;
  const ApplicationRequirements m_appRequirements;
  SwapChainRequirements m_swapChainRequirements;
  Memory::Allocator& m_allocator;

  U32 m_frameCount;
};
} // namespace Azura
