#include "Camera/PolarCamera.h"
#include "Math/Transform.h"
#include "Math/Geometry.h"
#include "Utils/Macros.h"

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

PolarCamera::PolarCamera(U32 width, U32 height, float thethaDeg, float phiDeg, float zoom)
  : Camera(width, height), m_theta(Math::ToRadians(thethaDeg)), m_phi(Math::ToRadians(phiDeg)), m_zoom(zoom) {
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


  m_view = Transform::LookAt(m_ref, m_eye, m_up);
  m_proj = Transform::Perspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);

  m_viewProj = m_proj * m_view;
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
  Recompute();
}

void PolarCamera::OnKeyEvent(KeyEvent keyEvent) {
  if (keyEvent.m_internalType == KeyEventType::KeyPress) {
    switch (keyEvent.m_key) {
    case KeyboardKey::W:
      m_moveUpFactor = 1;
      break;

    case KeyboardKey::S:
      m_moveUpFactor = -1;
      break;

    case KeyboardKey::D:
      m_moveRightFactor = -1;
      break;

    case KeyboardKey::A:
      m_moveRightFactor = 1;
      break;

    case KeyboardKey::Unmapped:
    case KeyboardKey::Esc:
    default:
      break;
    }
  } else if (keyEvent.m_internalType == KeyEventType::KeyRelease) {
    switch (keyEvent.m_key) {
    case KeyboardKey::W:
      m_moveUpFactor = 0;
      break;

    case KeyboardKey::S:
      m_moveUpFactor = 0;
      break;

    case KeyboardKey::D:
      m_moveRightFactor = 0;
      break;

    case KeyboardKey::A:
      m_moveRightFactor = 0;
      break;

    case KeyboardKey::Unmapped:
    case KeyboardKey::Esc:
    default:
      break;
    }
  }
}

void PolarCamera::SetZoom(float value) {
  m_zoom = value;
}

void PolarCamera::SetZoomAndRecompute(float value) {
  SetZoom(value);
  Recompute();
}

void PolarCamera::RotateAboutUp(float deg) {
  m_theta += Math::ToRadians(deg);
}

void PolarCamera::RotateAboutRight(float deg) {
  m_phi += Math::ToRadians(deg);
}

void PolarCamera::Update(float timeDelta) {
  const float distance = m_stepSize * timeDelta;

  bool needsRecompute = false;

  if (m_moveUpFactor != 0) {
    RotateAboutRight(distance * m_moveUpFactor);
    needsRecompute = true;
  }

  if (m_moveRightFactor != 0) {
    RotateAboutUp(distance * m_moveRightFactor);
    needsRecompute = true;
  }

  if (needsRecompute) {
    Recompute();
  }
}
} // namespace Azura
