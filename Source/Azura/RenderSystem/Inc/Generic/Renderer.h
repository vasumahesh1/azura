#pragma once

#include "Constants.h"
#include "RawStorageFormat.h"
#include "GenericTypes.h"
#include "Containers/Vector.h"
#include "Drawable.h"

namespace Azura {
struct DrawablePoolCreateInfo;

struct Bounds3D {
  uint32_t m_width{0};
  uint32_t m_height{0};
  uint32_t m_depth{0};
};

struct Bounds2D {
  uint32_t m_width{0};
  uint32_t m_height{0};
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

struct SwapChainRequirement {
  RawStorageFormat m_format{};
  Bounds2D m_extent{};
  ColorSpace m_colorSpace{};
};

class Renderer {
public:
  Renderer(ApplicationInfo appInfo, const DeviceRequirements& deviceRequirements, Memory::Allocator& allocator);
  virtual ~Renderer() = default;

  Renderer(const Renderer& other) = delete;
  Renderer(Renderer&& other) = delete;
  Renderer& operator=(const Renderer& rhs) = delete;
  Renderer& operator=(Renderer&& rhs) = delete;

  virtual U32 CreateDrawablePool(const DrawablePoolCreateInfo& createInfo) = 0;
  virtual DrawablePool& GetDrawablePool(U32 id) = 0;

protected:
  const ApplicationInfo& GetApplicationInfo() const;
  const DeviceRequirements& GetDeviceRequirements() const;
  Memory::Allocator& GetAllocator() const;

private:
  const ApplicationInfo m_applicationInfo;
  const DeviceRequirements m_deviceRequirements;
  Memory::Allocator& m_allocator;
};
}
