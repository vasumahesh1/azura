
.. _program_listing_file_Source_Dibella_Inc_Core_Constants.h:

Program Listing for File Constants.h
====================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Dibella_Inc_Core_Constants.h>` (``Source\Dibella\Inc\Core\Constants.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "Types.h"
   
   namespace Dibella {
   constexpr Float SHADOW_EPSILON    = 0.0001f;
   constexpr Float RAY_EPSILON       = 0.000005f;
   constexpr Float PI                = 3.14159265358979323846f;
   constexpr Float TWO_PI            = 6.28318530717958647692f;
   constexpr Float INV_PI            = 0.31830988618379067154f;
   constexpr Float INV2_PI           = 0.15915494309189533577f;
   constexpr Float INV4_PI           = 0.07957747154594766788f;
   constexpr Float PI_OVER2          = 1.57079632679489661923f;
   constexpr Float PI_OVER4          = 0.78539816339744830961f;
   constexpr Float SQRT2             = 1.41421356237309504880f;
   constexpr Float ONE_MINUS_EPSILON = 0.99999994f;
   constexpr Float EPSILON           = 0.000001f;
   
   // BxDFs a single BSDF can contain at max
   constexpr int MAX_BDSF_BXDF{8};
   }  // namespace Dibella
