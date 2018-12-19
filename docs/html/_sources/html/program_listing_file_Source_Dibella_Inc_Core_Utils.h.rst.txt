
.. _program_listing_file_Source_Dibella_Inc_Core_Utils.h:

Program Listing for File Utils.h
================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Dibella_Inc_Core_Utils.h>` (``Source\Dibella\Inc\Core\Utils.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include <algorithm>
   #include "Constants.h"
   #include "Types.h"
   
   namespace Dibella {
   inline bool SameHemisphere(const Vector3f& w, const Vector3f& wp) {
     return w.z * wp.z > 0;
   }
   
   inline Float ToRadians(Float degrees) {
     return degrees * (PI / 180.0f);
   }
   
   inline Point2i UVToPixelSpace(Point2f uv, SizeType width, SizeType height) {
     Point2i result;
     result.x = std::min(width * uv.x, width - 1.0f);
     result.y = std::min(height * (1.0f - uv.y), height - 1.0f);
   
     return result;
   }
   }  // namespace Dibella
