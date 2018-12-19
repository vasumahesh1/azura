
.. _program_listing_file_Source_Math_Src_Geometry.cpp:

Program Listing for File Geometry.cpp
=====================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Math_Src_Geometry.cpp>` (``Source\Math\Src\Geometry.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Math/Geometry.h"
   
   namespace Azura {
   float Math::ToRadians(const float degrees) {
     return (PI / 180.0f) * degrees;
   }
   
   float Math::ToAngle(const float radians) {
     return (180.0f / PI) * radians;
   }
   } // namespace Azura
