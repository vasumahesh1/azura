#pragma once
#include "Core.h"

namespace Azura {
namespace Transform {
Matrix4f LookAt(const Vector3f& at, const Vector3f& eye, const Vector3f& worldUp);
Matrix4f Perspective(float fovY, float aspect, float zNear, float zFar);
}
}
