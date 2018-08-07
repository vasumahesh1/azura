#include "Ray.h"

namespace Dibella {

  Ray::Ray(const Point3f& o, const Vector3f& d)
    : mOrigin(o),
      mDirection(d) {}

  Ray::Ray(const Vector4f& o, const Vector4f& d)
    : mOrigin(o.xyz()),
      mDirection(d.xyz()) {}

  Ray Ray::GetTransformedCopy(const Matrix4f& transform) const {
    auto originVec4    = Vector4f(mOrigin, 1.0f);
    auto directionVec4 = Vector4f(mDirection, 0.0f);

    originVec4    = transform * originVec4;
    directionVec4 = transform * directionVec4;

    return {originVec4, directionVec4};
  }
}
