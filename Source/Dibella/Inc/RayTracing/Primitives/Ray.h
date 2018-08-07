#pragma once
#include "../../Core/Types.h"

namespace Dibella {

  class Ray {
  public:
    Ray(const Point3f &o, const Vector3f &d);
    Ray(const Vector4f &o, const Vector4f &d);

    //Return a copy of this ray that has been transformed
    //by the input transformation matrix.
    Ray GetTransformedCopy(const Matrix4f &T) const;

  private:
    Point3f mOrigin;
    Vector3f mDirection;
  };
}
