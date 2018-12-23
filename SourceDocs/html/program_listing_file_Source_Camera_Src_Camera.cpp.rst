
.. _program_listing_file_Source_Camera_Src_Camera.cpp:

Program Listing for File Camera.cpp
===================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Camera_Src_Camera.cpp>` (``Source\Camera\Src\Camera.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

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
     assert(value < m_farClip);
     m_nearClip = value;
   }
   
   void Camera::SetFarClip(const float value) {
     assert(value > m_nearClip);
     m_farClip = value;
   }
   
   float Camera::GetNearClip() const {
     return m_nearClip;
   }
   
   float Camera::GetFarClip() const {
     return m_farClip;
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
   
   Matrix4f Camera::GetViewMatrix() const {
     return m_view;
   }
   
   Matrix4f Camera::GetProjMatrix() const {
     return m_proj;
   }
   } // namespace Azura
