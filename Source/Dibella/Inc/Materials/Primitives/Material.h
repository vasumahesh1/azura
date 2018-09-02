#pragma once
#include "../../Core/Types.h"
#include "../../Core/Utils.h"

namespace Dibella {
class ImageHandler;
class Allocator;
class Intersection;

class Material {
 public:
  virtual ~Material()                 = default;
  Material(const Material& other)     = default;
  Material(Material&& other) noexcept = default;
  Material& operator=(const Material& other) = default;
  Material& operator=(Material&& other) noexcept = default;

  virtual void GenerateBSDF(Intersection* isect, Allocator* allocator) const = 0;

  // TODO: [REMOVAL] Useless extra call?
  static Color3f GetImageColor(const Point2f& uv, const ImageHandler* handle);
};
}  // namespace Dibella
