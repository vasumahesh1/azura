#pragma once
#include "Camera.h"

namespace Azura {

class PolarCamera final : public Camera {
public:
  PolarCamera(U32 width, U32 height);

  PolarCamera(U32 width, U32 height, float thethaDeg, float phiDeg, float zoom);

  void Recompute() override;
  void OnMouseEvent(MouseEvent mouseEvent) override;

  void SetZoom(float value);
  void SetZoomAndRecompute(float value);

  void RotateAboutUp(float deg);
  void RotateAboutRight(float deg);

private:
  float m_theta{0.0f};
  float m_phi{0.0f};
  float m_zoom{4.0f};

  // Axes of Camera in world space
  Vector3f m_up;
  Vector3f m_right;
  Vector3f m_look;
};

} // namespace Azura
