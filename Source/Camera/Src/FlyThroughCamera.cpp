#include "Camera/FlyThroughCamera.h"
#include "Math/Transform.h"
#include "Math/Geometry.h"
#include "Core/Events.h"

namespace Azura {
namespace {
const Vector3f UNIT_LOOK  = Vector3f(0, 0, 1);
const Vector3f UNIT_RIGHT = Vector3f(1, 0, 0);
const Vector3f UNIT_UP    = Vector3f(0, 1, 0);
} // namespace

FlyThroughCamera::FlyThroughCamera(U32 width, U32 height)
  : Camera(width, height),
    m_cachedMouseX(width / 2.0f),
    m_cachedMouseY(height / 2.0f) {
  FlyThroughCamera::Recompute();
}

void FlyThroughCamera::Recompute() {
  const Matrix4f transform = Matrix4f::FromRotationMatrix(Matrix4f::RotationY(m_theta)) *
                             Matrix4f::FromRotationMatrix(Matrix3f::RotationX(m_phi));

  m_look  = (transform * Vector4f(UNIT_LOOK, 0)).xyz();
  m_up    = (transform * Vector4f(UNIT_UP, 0)).xyz();
  m_right = (transform * Vector4f(UNIT_RIGHT, 0)).xyz();

  m_ref = m_look.Normalized();
  m_ref += m_eye;

  m_view = Transform::LookAt(m_ref, m_eye, m_up);
  m_proj = Transform::Perspective(m_fovY, m_aspect, m_nearClip, m_farClip);

  m_viewProj    = m_proj * m_view;
  m_invViewProj = m_viewProj.Inverse();
}

void FlyThroughCamera::OnMouseEvent(MouseEvent mouseEvent) {
  if (mouseEvent.m_internalType != MouseEventType::MouseUpdate) {
    return;
  }

  const float currentX = mouseEvent.m_eventX;
  const float currentY = mouseEvent.m_eventY;

  const float diffX = currentX - m_cachedMouseX;
  const float diffY = currentY - m_cachedMouseY;

  RotateAboutRight(diffY * m_sensitivity * 2.0f);
  RotateAboutUp(diffX * m_sensitivity * 2.0f);

  Recompute();
}

void FlyThroughCamera::OnKeyEvent(KeyEvent keyEvent) {
  if (keyEvent.m_internalType == KeyEventType::KeyPress) {
    switch (keyEvent.m_key) {
      case KeyboardKey::W:
        m_moveForwardFactor = 1;
        break;

      case KeyboardKey::S:
        m_moveForwardFactor = -1;
        break;

      case KeyboardKey::D:
        m_moveRightFactor = 1;
        break;

      case KeyboardKey::A:
        m_moveRightFactor = -1;
        break;

      case KeyboardKey::Unmapped:
      case KeyboardKey::Esc:
      default:
        break;
    }
  } else if (keyEvent.m_internalType == KeyEventType::KeyRelease) {
    switch (keyEvent.m_key) {
      case KeyboardKey::W:
        m_moveForwardFactor = 0;
        break;

      case KeyboardKey::S:
        m_moveForwardFactor = 0;
        break;

      case KeyboardKey::D:
        m_moveRightFactor = 0;
        break;

      case KeyboardKey::A:
        m_moveRightFactor = 0;
        break;

      case KeyboardKey::Unmapped:
      case KeyboardKey::Esc:
      case KeyboardKey::Up:
      case KeyboardKey::Down:
      case KeyboardKey::Left:
      case KeyboardKey::Right:
      case KeyboardKey::T:
      case KeyboardKey::Y:
      default:
        break;
    }
  }
}

void FlyThroughCamera::SetTranslationStepSize(float amount) {
  m_stepSize = amount;
}

void FlyThroughCamera::RotateAboutUp(float degrees) {
  m_theta += Math::ToRadians(degrees);

  if (m_theta >= Math::TWO_PI) {
    m_theta = m_theta - Math::TWO_PI;
  }

  if (m_theta < 0) {
    m_theta = Math::TWO_PI + m_theta;
  }
}

void FlyThroughCamera::SetAngleAboutRight(float ndcY) {
  const float deg = ndcY * 89 * -1;
  m_phi           = Math::ToRadians(deg);
}

void FlyThroughCamera::RotateAboutRight(float degrees) {
  m_phi += Math::ToRadians(degrees);

  if (m_phi >= Math::PI_OVER2) {
    m_phi = Math::PI_OVER2 - Math::ONE_DEGREE_RADIAN;
  }

  if (m_phi <= -Math::PI_OVER2) {
    m_phi = -Math::PI_OVER2 + Math::ONE_DEGREE_RADIAN;
  }
}

void FlyThroughCamera::TranslateAlongLook(float amt) {
  const Vector3f translation = m_look * amt;

  m_eye += translation;
  m_ref += translation;
}

void FlyThroughCamera::TranslateAlongRight(float amt) {
  const Vector3f translation = m_right * amt;

  m_eye += translation;
  m_ref += translation;
}

void FlyThroughCamera::Update(float timeDelta) {
  const float distance = m_stepSize * timeDelta;

  bool needsRecompute = false;

  if (m_moveForwardFactor != 0) {
    TranslateAlongLook(distance * m_moveForwardFactor);
    needsRecompute = true;
  }

  if (m_moveRightFactor != 0) {
    TranslateAlongRight(distance * m_moveRightFactor);
    needsRecompute = true;
  }

  if (needsRecompute) {
    Recompute();
  }
}

} // namespace Azura
