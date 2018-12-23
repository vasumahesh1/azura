#pragma once
#include "Types.h"
#include "Math/Core.h"
#include "Core/Events.h"

namespace Azura {

class Camera {
public:
  /**
   * \brief      Constructs the camera with the given width and height.
   *
   * \details    The height & width provided are used in the projection matrix computation.
   *
   * \param[in]  width   The film width
   * \param[in]  height  The film height
   */
  Camera(U32 width, U32 height);

  virtual ~Camera() = default;

  Camera(const Camera& other) = delete;
  Camera(Camera&& other) noexcept = delete;
  Camera& operator=(const Camera& other) = delete;
  Camera& operator=(Camera&& other) noexcept = delete;

  /**
   * \brief      Sets the sensitivity of the camera.
   *
   * \details    Sensitivity is an independent parameter and may be used by the dervied class in a particular way.
   *
   * \param[in]  value  The sensitivity value
   */
  void SetSensitivity(float value);

  /**
   * \brief      Query the sensitivity of the camera
   *
   * \return     The sensitivity of the camera.
   */
  float GetSensitivity() const;

  /**
   * \brief      Sets the near clip plane for the camera.
   *
   * \param[in]  value  The value
   */
  void SetNearClip(float value);

  /**
   * \brief      Sets the far clip plane for the camera.
   *
   * \param[in]  value  The value
   */
  void SetFarClip(float value);

  /**
   * \brief      Query the near clip plane value of the camera.
   *
   * \return     The near clip value.
   */
  float GetNearClip() const;

  /**
   * \brief      Query the far clip plane value of the camera.
   *
   * \return     The far clip value.
   */
  float GetFarClip() const;

  /**
   * \brief      Sets the vertical field of view of the camera.
   *
   * \param[in]  value  The value
   */
  void SetFOVY(float value);

  /**
   * \brief      Sets the camera position.
   *
   * \ingroup    Camera
   *
   * \param[in]  value  3D world position
   */
  void SetPosition(Vector3f value);

  /**
   * \brief      Sets the reference point for the camera.
   *
   * \details    The reference point is the camera's target.
   *
   * \param[in]  value  3D world position
   */
  void SetReferencePoint(Vector3f value);

  /**
   * \brief      Query to get the current camera position.
   *
   * \return     The position.
   */
  Vector3f GetPosition() const;

  /**
   * \brief      Gets the view projection matrix for the camera.
   *
   * \return     The view projection matrix.
   */
  Matrix4f GetViewProjMatrix() const;
  
  /**
   * \brief      Gets the inverse of the view projection matrix for the camera.
   *
   * \return     The inverse of view projection matrix.
   */
  Matrix4f GetInvViewProjMatrix() const;

  /**
   * \brief      Gets the view matrix.
   *
   * \return     The view matrix.
   */
  Matrix4f GetViewMatrix() const;

  /**
   * \brief      Gets the projection matrix.
   *
   * \return     The projection matrix.
   */
  Matrix4f GetProjMatrix() const;

  /**
   * \brief      Tells the camera to update its matrices & other properties.
   *
   * \details    Cameras in Azura don't recompute always.
   * For example, you maybe wanted to move the camera first, then rotate it,
   * maybe then shift it due to some physics. At the end, One can call recompute only once to get
   * the final view projection matrix.
   * 
   * Recompute will compute the view & projection matrices (and their combinations).
   */
  virtual void Recompute() = 0;

  /**
   * \brief      Update based on Window events
   *
   * \details    Azura cameras have the ability to listen to keyboard events or mouse events. They can then perform
   * movements based on the `timeDelta` passed. This is highly useful as every PolarCamera or FlyThroughCamera are generally
   * the same with some modification for the screen, fovy etc.
   * 
   * They all respond to a basic set of movements. However, the user can modify the keyboard keys for each camera.
   * But this is highly dependent on the application using the camera. For example, WASD is the default for all cameras.
   * But one can change those keys to maybe say the arrow keys.
   *
   * \param[in]  timeDelta  The time delta
   */
  virtual void Update(float timeDelta) = 0;
  virtual void OnMouseEvent(MouseEvent mouseEvent) = 0;
  virtual void OnKeyEvent(KeyEvent keyEvent) = 0;

protected:
  U32 m_width;
  U32 m_height;

  float m_aspect;

  float m_sensitivity{0.5f};

  float m_nearClip{0.0001f};
  float m_farClip{1000.0f};
  float m_fovY{45.0f};

  Vector3f m_eye{0, 0, 0};
  Vector3f m_ref{0, 0, 0};

  Matrix4f m_view;
  Matrix4f m_proj;
  Matrix4f m_viewProj;
  Matrix4f m_invViewProj;
};

} // namespace Azura
