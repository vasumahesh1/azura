#pragma once
#include "Math/Core.h"

namespace Azura {
struct PointLight {
  Vector3f m_position;
  float m_radius;

  Vector3f m_color;
  float pad;
};
}
