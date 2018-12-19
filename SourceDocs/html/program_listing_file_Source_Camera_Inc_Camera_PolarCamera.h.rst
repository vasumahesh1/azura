
.. _program_listing_file_Source_Camera_Inc_Camera_PolarCamera.h:

Program Listing for File PolarCamera.h
======================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Camera_Inc_Camera_PolarCamera.h>` (``Source\Camera\Inc\Camera\PolarCamera.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "Camera.h"
   
   namespace Azura {
   
   class PolarCamera final : public Camera {
   public:
     PolarCamera(U32 width, U32 height);
   
     PolarCamera(U32 width, U32 height, float thethaDeg, float phiDeg, float zoom);
   
     void Recompute() override;
     void OnMouseEvent(MouseEvent mouseEvent) override;
     void OnKeyEvent(KeyEvent keyEvent) override;
   
     void SetZoom(float value);
     void SetZoomAndRecompute(float value);
   
     void RotateAboutUp(float deg);
     void RotateAboutRight(float deg);
   
     void TranslateAlongLook(float amount);
   
     void SetStepSize(float value);
   
     void Update(float timeDelta) override;
   
   private:
     float m_theta{0.0f};
     float m_phi{0.0f};
     float m_zoom{4.0f};
   
     // Axes of Camera in world space
     Vector3f m_up;
     Vector3f m_right;
     Vector3f m_look;
   
     float m_stepSize{10.0f};
   
     int m_moveUpFactor{0};
     int m_moveRightFactor{0};
     int m_zoomFactor{0};
   };
   
   } // namespace Azura
