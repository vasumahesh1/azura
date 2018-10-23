#include "Camera/Camera.h"

namespace Azura {
Camera::Camera(U32 width, U32 height)
  : m_width(width),
    m_height(height),
    m_aspect(width / float(height)) {
}

void Camera::SetSensitivity(float value) {
  m_sensitivity = value;
}

float Camera::GetSensitivity() const {
  return m_sensitivity;
}

void Camera::SetNearClip(const float value) {
  m_nearClip = value;
}

void Camera::SetFarClip(const float value) {
  m_farClip = value;
}

void Camera::SetFOVY(float value) {
  m_fovY = value;
}

void Camera::SetPosition(Vector3f value) {
  m_eye = value;
}

void Camera::SetReferencePoint(Vector3f value) {
  m_ref = value;
}

Vector3f Camera::GetPosition() const {
  return m_eye;
}

Matrix4f Camera::GetViewProjMatrix() const {
  return m_viewProj;
}

Matrix4f Camera::GetInvViewProjMatrix() const {
  return m_invViewProj;
}
} // namespace Azura
