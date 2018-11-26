#pragma once

#include "Math/Core.h"
#include "Core/Events.h"

namespace Azura {
namespace Math {

class TransformComponent {
public:

  TransformComponent();
  explicit TransformComponent(const Matrix4f& transform);

  static TransformComponent FromTranslationVector(const Vector3f& translation);
  void SetStepSize(float value);

  void OnKeyEvent(KeyEvent keyEvent);
  void Update(float timeDelta);

  void SetUpKey(KeyboardKey key);
  void SetDownKey(KeyboardKey key);
  void SetLeftKey(KeyboardKey key);
  void SetRightKey(KeyboardKey key);
  void SetForwardKey(KeyboardKey key);
  void SetBackwardKey(KeyboardKey key);

  const Matrix4f& GetTransform() const;

private:
  float m_sensitivity{1.0f};
  float m_stepSize{10.0f};

  Matrix4f m_transform;

  KeyboardKey m_forwardKey{KeyboardKey::W};
  KeyboardKey m_backwardKey{KeyboardKey::S};
  KeyboardKey m_upKey{KeyboardKey::R};
  KeyboardKey m_downKey{KeyboardKey::T};
  KeyboardKey m_leftKey{KeyboardKey::A};
  KeyboardKey m_rightKey{KeyboardKey::D};

  int m_moveForwardFactor{0};
  int m_moveUpFactor{0};
  int m_moveRightFactor{0};
};

} // namespace Math
} // namespace Azura
