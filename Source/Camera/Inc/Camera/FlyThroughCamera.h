#pragma once
#include "Camera.h"
#include "Types.h"
#include "Math/Core.h"

namespace Azura {
struct MouseEvent;

class FlyThroughCamera final : public Camera {
public:
  FlyThroughCamera(U32 width, U32 height);

  void Recompute() override;
  void OnMouseEvent(MouseEvent mouseEvent) override;
  void OnKeyEvent(KeyEvent keyEvent) override;

  void SetTranslationStepSize(float amount);

  void RotateAboutUp(float degrees);

  void SetAngleAboutRight(float ndcY);

  void RotateAboutRight(float degrees);

  void TranslateAlongLook(float amt);

  void TranslateAlongRight(float amt);

private:
  Vector3f m_right{1, 0, 0};
  Vector3f m_look{0, 0, 1};
  Vector3f m_up{0, 1, 0};

  float m_theta{0.0f};
  float m_phi{0.0f};
  float m_stepSize{10.0f};

  float m_cachedMouseX;
  float m_cachedMouseY;

};

} // namespace Azura
