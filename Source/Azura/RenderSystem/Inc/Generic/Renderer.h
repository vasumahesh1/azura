#pragma once

#include <vector>
#include "Constants.h"
#include "RawStorageFormat.h"
#include "GenericTypes.h"

namespace Azura {
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
  Renderer(const ApplicationInfo& appInfo, const DeviceRequirements& deviceRequirements);
  virtual ~Renderer() = default;

  Renderer(const Renderer& other) = delete;
  Renderer(Renderer&& other) = delete;
  Renderer& operator=(const Renderer& rhs) = delete;
  Renderer& operator=(Renderer&& rhs) = delete;

  virtual void AddRenderEntity() = 0;

private:
};
}
