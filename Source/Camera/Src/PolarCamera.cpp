#include "Camera/PolarCamera.h"
#include "Math/Transform.h"
#include "Math/Geometry.h"

namespace Azura {
namespace {
const Vector3f UNIT_LOOK  = Vector3f(0, 0, 1);
const Vector3f UNIT_RIGHT = Vector3f(1, 0, 0);
const Vector3f UNIT_UP    = Vector3f(0, 1, 0);
const Vector3f UNIT_EYE   = Vector3f(0, 0, 0);
} // namespace

PolarCamera::PolarCamera(U32 width, U32 height)
  : Camera(width, height) {
  PolarCamera::Recompute();
}

void PolarCamera::Recompute() {
  const Matrix4f transform = Matrix4f::FromRotationMatrix(Matrix4f::RotationY(m_theta)) *
                       Matrix4f::FromRotationMatrix(Matrix3f::RotationX(m_phi)) *
                       Matrix4f::FromTranslationVector(Vector3f(0, 0, 1) * m_zoom);

  m_look = Transform::Downgrade(transform * Vector4f(UNIT_LOOK, 0.0f));
  m_up = Transform::Downgrade(transform * Vector4f(UNIT_UP, 0.0f));
  m_right = Transform::Downgrade(transform * Vector4f(UNIT_RIGHT, 0.0f));

  // Position must be affected by Translation
  m_eye = Transform::Downgrade(transform * Vector4f(UNIT_EYE, 1.0f));


  const Matrix4f view = Transform::LookAt(m_ref, m_eye, m_up);
  const Matrix4f proj = Transform::Perspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);

  m_viewProj = proj * view;
  m_invViewProj = m_viewProj.Inverse();
}

void PolarCamera::OnMouseEvent(MouseEvent mouseEvent) {
  if (mouseEvent.m_internalType != MouseEventType::Drag)
  {
    return;
  }

  const float anglePerPixel = 5;
  RotateAboutUp(mouseEvent.m_eventX * anglePerPixel * m_sensitivity);
  RotateAboutRight(mouseEvent.m_eventY * anglePerPixel * m_sensitivity);
}

void PolarCamera::RotateAboutUp(float deg) {
  m_theta += Math::ToRadians(deg);
  Recompute();
}

void PolarCamera::RotateAboutRight(float deg) {
  m_phi += Math::ToRadians(deg);
  Recompute();
}
} // namespace Azura
