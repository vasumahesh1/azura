
.. _program_listing_file_Source_Math_Inc_Math_Core.h:

Program Listing for File Core.h
===============================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Math_Inc_Math_Core.h>` (``Source\Math\Inc\Math\Core.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include <mathfu/matrix.h>
   #include "Types.h"
   
   namespace Azura
   {
     using Vector4d = mathfu::Vector<double, 4>;
     using Vector4f = mathfu::Vector<float, 4>;
     using Vector4i = mathfu::Vector<int, 4>;
     using Vector4u = mathfu::Vector<U32, 4>;
   
     using Vector3d = mathfu::Vector<double, 3>;
     using Vector3f = mathfu::Vector<float, 3>;
     using Vector3i = mathfu::Vector<int, 3>;
     using Vector3u = mathfu::Vector<U32, 3>;
   
     using Vector2d = mathfu::Vector<double, 2>;
     using Vector2f = mathfu::Vector<float, 2>;
     using Vector2i = mathfu::Vector<int, 2>;
     using Vector2u = mathfu::Vector<U32, 2>;
   
     using Matrix4d = mathfu::Matrix<double, 4, 4>;
     using Matrix4f = mathfu::Matrix<float, 4, 4>;
     using Matrix4i = mathfu::Matrix<int, 4, 4>;
     using Matrix4u = mathfu::Matrix<U32, 4, 4>;
   
     using Matrix3d = mathfu::Matrix<double, 3, 3>;
     using Matrix3f = mathfu::Matrix<float, 3, 3>;
     using Matrix3i = mathfu::Matrix<int, 3, 3>;
     using Matrix3u = mathfu::Matrix<U32, 3, 3>;
   
     using Matrix2d = mathfu::Matrix<double, 2, 2>;
     using Matrix2f = mathfu::Matrix<float, 2, 2>;
     using Matrix2i = mathfu::Matrix<int, 2, 2>;
     using Matrix2u = mathfu::Matrix<U32, 2, 2>;
   
     using Color4d = Vector4d;
     using Color4f = Vector4f;
     using Color4i = Vector4i;
     using Color4u = Vector4u;
   
     using Color3d = Vector3d;
     using Color3f = Vector3f;
     using Color3i = Vector3i;
     using Color3u = Vector3u;
   
     using Normal3d = Vector3d;
     using Normal3f = Vector3f;
   
     enum
     {
       AxisX = 0,
       AxisY = 1,
       AxisZ = 2,
       AxisW = 3
     };
   
   
     constexpr float EPSILON = 0.000001f;
   }
