#pragma once

#include <string>
#include <cstddef>
#include "mathfu/vector.h"
#include "mathfu/matrix.h"
#include <ToolsCore.h>

namespace Dibella {
  using Color3f = mathfu::Vector<float, 3>;
  using Normal3f = mathfu::Vector<float, 3>;
  using Vector4f = mathfu::Vector<float, 4>;
  using Vector3f = mathfu::Vector<float, 3>;
  using Point3f = mathfu::Vector<float, 3>;
  using Vector2f = mathfu::Vector<float, 2>;
  using Point2f = mathfu::Vector<float, 2>;
  using Vector3i = mathfu::Vector<int, 3>;
  using Point3i = mathfu::Vector<int, 3>;
  using Vector2i = mathfu::Vector<int, 2>;
  using Point2i = mathfu::Vector<int, 2>;
  using Float = float;

  using Matrix3f = mathfu::Matrix<float, 3, 3>;
  using Matrix4f = mathfu::Matrix<float, 4, 4>;

  using DString = std::string;

  using SizeType = std::size_t;

  using U64 = EngineTools::U64;
}
