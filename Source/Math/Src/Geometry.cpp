#include "Math/Geometry.h"

namespace Azura {
float Math::ToRadians(const float degrees) {
  return (PI / 180.0f) * degrees;
}

float Math::ToAngle(const float radians) {
  return (180.0f / PI) * radians;
}
} // namespace Azura
