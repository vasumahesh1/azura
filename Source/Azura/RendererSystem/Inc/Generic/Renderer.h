#pragma once

#include <cstdint>

#include <array>
#include <vector>

#include "Containers/Vector.h"
#include "Constants.h"

namespace Azura {
struct DeviceRequirements {
  bool m_discreteGPU{true};
  bool m_transferQueue{false};
  bool m_float64{false};
  bool m_int64{false};
};

struct ApplicationInfo
{
    std::string m_name;
    Version m_version;
};

class Renderer {
public:
  Renderer(const ApplicationInfo& appInfo, const DeviceRequirements &deviceRequirements);
  virtual ~Renderer() = default;

  Renderer(const Renderer &other) = delete;
  Renderer(Renderer &&other) = delete;
  Renderer &operator=(const Renderer &rhs) = delete;
  Renderer &operator=(Renderer &&rhs) = delete;

  virtual void AddRenderEntity() = 0;

private:
};
}
