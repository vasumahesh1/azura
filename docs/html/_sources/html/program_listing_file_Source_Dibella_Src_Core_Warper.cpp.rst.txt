
.. _program_listing_file_Source_Dibella_Src_Core_Warper.cpp:

Program Listing for File Warper.cpp
===================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Dibella_Src_Core_Warper.cpp>` (``Source\Dibella\Src\Core\Warper.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Warper.h"
   #include "Constants.h"
   #include "Utils.h"
   
   namespace Dibella {
   
   Vector3f Warper::SquareToDiskUniform(const Vector2f& sample) {
     const Float radius = std::sqrt(sample.x);
     const Float angle  = 2.0f * PI * sample.y;
   
     return Vector3f(radius * std::cos(angle), radius * std::sin(angle), 0);
   }
   
   Vector3f Warper::SquareToDiskConcentric(const Vector2f& sample) {
     Float radius, angle;
   
     const Float a = 2.0f * sample.x - 1.0f;
     const Float b = 2.0f * sample.y - 1.0f;
   
     if (a > -b) {
       if (a > b) {
         radius = a;
         angle  = (PI / 4.0f) * (b / a);
       }
       else {
         radius = b;
         angle  = (PI / 4.0f) * (2.0f - (a / b));
       }
     }
     else {
       if (a < b) {
         radius = -a;
         angle  = (PI / 4.0f) * (4.0f + (b / a));
       }
       else {
         radius = -b;
         if (b != 0) {
           angle = (PI / 4.0f) * (6.0f - (a / b));
         }
         else {
           angle = 0.0f;
         }
       }
     }
   
     return Vector3f(radius * std::cos(angle), radius * std::sin(angle), 0);
   }
   
   Float Warper::SquareToDiskPDF(const Vector3f& sample) {
     return INV_PI;
   }
   
   Vector3f Warper::SquareToSphereUniform(const Vector2f& sample) {
     const Float finalZ = 1.0f - 2.0f * sample.x;
   
     const Float radius = std::sqrt(std::fmax(0.0f, 1.0f - finalZ * finalZ));
     const Float angle  = 2.0f * PI * sample.y;
   
     return Vector3f(radius * std::cos(angle), radius * std::sin(angle), finalZ);
   }
   
   Float Warper::SquareToSphereUniformPDF(const Vector3f& sample) {
     return INV4_PI;
   }
   
   Vector3f Warper::SquareToSphereCapUniform(const Vector2f& sample, const Float thetaMin) {
     const Float capAngle = thetaMin;
     const Float ratio    = (360.0f - capAngle) / 360.0f;
   
     const Float finalZ = ((1.0f - ratio) * sample.x) + ratio;
   
     const Float radius = std::sqrt(std::fmax(0.0f, 1.0f - finalZ * finalZ));
     const Float angle  = 2.0f * PI * sample.y;
   
     return Vector3f(radius * std::cos(angle), radius * std::sin(angle), finalZ);
   }
   
   Float Warper::SquareToSphereCapUniformPDF(const Vector3f& sample, const Float thetaMin) {
     // 2pi * (1 - cos(theta))
     Float sa = (1.0 - std::cos(ToRadians(180.0f - thetaMin)));
     return 0.5f * INV4_PI * 1.0 / (sa);
   }
   
   Vector3f Warper::SquareToHemisphereUniform(const Vector2f& sample) {
     const Float finalZ = sample.x;
   
     const Float radius = std::sqrt(std::fmax(0.0f, 1.0f - finalZ * finalZ));
     const Float angle  = 2.0f * PI * sample.y;
   
     return Vector3f(radius * std::cos(angle), radius * std::sin(angle), finalZ);
   }
   
   Float Warper::SquareToHemisphereUniformPDF(const Vector3f& sample) {
     return INV2_PI;
   }
   
   Vector3f Warper::SquareToHemisphereCosine(const Vector2f& sample) {
     const Vector3f val = SquareToDiskConcentric(sample);
     const Float finalZ = sqrt(std::fmax(0.0f, 1.0f - val.x * val.x - val.y * val.y));
   
     return Vector3f(val.x, val.y, finalZ);
   }
   
   Float Warper::SquareToHemisphereCosinePDF(const Vector3f& sample) {
     return INV_PI * std::abs(sample.z);
   }
   }  // namespace Dibella
