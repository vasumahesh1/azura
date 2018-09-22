#include "Math/Transform.h"

#ifdef AZURA_USE_RHS
const float MATHFU_HANDEDNESS = 1.0f;
#else
const float MATHFU_HANDEDNESS = -1.0f;
#endif

namespace Azura {
Matrix4f Transform::LookAt(const Vector3f& at, const Vector3f& eye, const Vector3f& worldUp) {
  return Matrix4f::LookAt(at, eye, worldUp, MATHFU_HANDEDNESS);
}

Matrix4f Transform::Perspective(const float fovY, const float aspect, const float zNear, const float zFar) {
  return Matrix4f::Perspective(fovY, aspect, zNear, zFar, MATHFU_HANDEDNESS);
}

Vector3f Transform::Downgrade(const Vector4f& vec) {
  return Vector3f(vec[0], vec[1], vec[2]);
}
} // namespace Azura
