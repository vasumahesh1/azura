#include "Transform.h"
#include "../../Core/Utils.h"

namespace Dibella {

Matrix3f AsMatrix3(Matrix4f matrix) {
  Matrix3f result;
  result.GetColumn(0) = matrix.GetColumn(0).xyz();
  result.GetColumn(1) = matrix.GetColumn(1).xyz();
  result.GetColumn(2) = matrix.GetColumn(2).xyz();
  return result;
}

Transform::Transform() : Transform(Vector3f(0.0f), Vector3f(0.0f), Vector3f(1.0f)) {}

Transform::Transform(const Vector3f& t, const Vector3f& r, const Vector3f& s)
    : mTranslation(t), mRotation(r), mScale(s) {
  auto translate  = Matrix4f();
  translate(0, 3) = mTranslation[0];
  translate(1, 3) = mTranslation[1];
  translate(2, 3) = mTranslation[2];
  translate(3, 3) = 1.0f;

  auto rotationMat3 = Matrix3f::RotationX(ToRadians(mRotation[0])) * Matrix3f::RotationY(ToRadians(mRotation[1]))
                      * Matrix3f::RotationZ(ToRadians(mRotation[2]));

  auto rotation         = Matrix4f();
  rotation.GetColumn(0) = Vector4f(rotationMat3.GetColumn(0), 0.0f);
  rotation.GetColumn(1) = Vector4f(rotationMat3.GetColumn(1), 0.0f);
  rotation.GetColumn(2) = Vector4f(rotationMat3.GetColumn(2), 0.0f);
  rotation.GetColumn(3) = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);

  auto scale  = Matrix4f::Identity();
  scale(0, 0) = mScale[0];
  scale(1, 1) = mScale[1];
  scale(2, 2) = mScale[2];

  mWorldTransform                 = translate * rotation * scale;
  mInverseWorldTransform          = mWorldTransform.Inverse();
  mInverseTransposeWorldTransform = AsMatrix3((mWorldTransform.Transpose()).Inverse());
}

const Matrix4f& Transform::Get() const {
  return mWorldTransform;
}

Matrix3f Transform::GetAsMatrix3() const {
  return AsMatrix3(mWorldTransform);
}

const Matrix4f& Transform::GetInverse() const {
  return mInverseWorldTransform;
}

const Matrix3f& Transform::GetInverseTranspose() const {
  return mInverseTransposeWorldTransform;
}
}  // namespace Dibella
