
.. _program_listing_file_Source_Math_Inc_Math_Transform.h:

Program Listing for File Transform.h
====================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Math_Inc_Math_Transform.h>` (``Source\Math\Inc\Math\Transform.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "Core.h"
   
   namespace Azura {
   namespace Transform {
   Matrix4f LookAt(const Vector3f& at, const Vector3f& eye, const Vector3f& worldUp);
   Matrix4f Perspective(float fovY, float aspect, float zNear, float zFar);
   
   Vector3f Downgrade(const Vector4f& vec);
   }
   }
