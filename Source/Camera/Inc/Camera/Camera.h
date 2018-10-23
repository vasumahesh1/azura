#pragma once
#include "Types.h"
#include "Math/Core.h"
#include "Core/Events.h"

namespace Azura {

class Camera {
public:
  Camera(U32 width, U32 height);

  virtual ~Camera() = default;

  Camera(const Camera& other) = delete;
  Camera(Camera&& other) noexcept = delete;
  Camera& operator=(const Camera& other) = delete;
  Camera& operator=(Camera&& other) noexcept = delete;

  void SetSensitivity(float value);
  float GetSensitivity() const;

  void SetNearClip(float value);
  void SetFarClip(float value);
  void SetFOVY(float value);
  void SetPosition(Vector3f value);
  void SetReferencePoint(Vector3f value);

  Vector3f GetPosition() const;

  Matrix4f GetViewProjMatrix() const;
  Matrix4f GetInvViewProjMatrix() const;

  virtual void Recompute() = 0;
  virtual void Update(float timeDelta) = 0;
  virtual void OnMouseEvent(MouseEvent mouseEvent) = 0;
  virtual void OnKeyEvent(KeyEvent keyEvent) = 0;

protected:
  U32 m_width;
  U32 m_height;

  float m_aspect;

  float m_sensitivity{0.5f};

  float m_nearClip{0.01f};
  float m_farClip{100.0f};
  float m_fovY{45.0f};

  Vector3f m_eye{0, 0, 0};
  Vector3f m_ref{0, 0, 0};

  Matrix4f m_viewProj;
  Matrix4f m_invViewProj;
};

} // namespace Azura
