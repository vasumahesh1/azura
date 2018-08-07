#pragma once
#include "../../Core/Types.h"

namespace Dibella {

  class Transform {
  public:
    Transform();
    Transform(const Vector3f& t, const Vector3f& r, const Vector3f& s);

    const Matrix4f& Get() const;
    Matrix3f GetAsMatrix3() const;
    const Matrix4f& GetInverse() const;
    const Matrix3f& GetInverseTranspose() const;

    Vector3f GetPosition() const { return mTranslation; }
    Vector3f GetRotation() const { return mRotation; }
    Vector3f GetScale() const { return mScale; }

  private:
    Vector3f mTranslation;
    Vector3f mRotation;
    Vector3f mScale;

    Matrix4f mWorldTransform;
    Matrix4f mInverseWorldTransform;
    Matrix3f mInverseTransposeWorldTransform;

  };
}
