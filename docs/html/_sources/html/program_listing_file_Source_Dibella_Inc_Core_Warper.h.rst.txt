
.. _program_listing_file_Source_Dibella_Inc_Core_Warper.h:

Program Listing for File Warper.h
=================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Dibella_Inc_Core_Warper.h>` (``Source\Dibella\Inc\Core\Warper.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "Types.h"
   
   namespace Dibella {
   
   class Warper {
    public:
     // Uniformly sample a vector on a 2D disk with radius 1, centered around the origin
     static Vector3f SquareToDiskUniform(const Vector2f& sample);
   
     // Concentrically sample a vector on a 2D disk with radius 1, centered around the origin
     static Vector3f SquareToDiskConcentric(const Vector2f& sample);
   
     static Float SquareToDiskPDF(const Vector3f& sample);
   
     // Uniformly sample a vector on the unit sphere with respect to solid angles
     static Vector3f SquareToSphereUniform(const Vector2f& sample);
   
     static Float SquareToSphereUniformPDF(const Vector3f& sample);
   
     static Vector3f SquareToSphereCapUniform(const Vector2f& sample, Float thetaMin);
   
     static Float SquareToSphereCapUniformPDF(const Vector3f& sample, Float thetaMin);
   
     // Uniformly sample a vector on the unit hemisphere around the pole (0,0,1) with respect to solid angles
     static Vector3f SquareToHemisphereUniform(const Vector2f& sample);
   
     static Float SquareToHemisphereUniformPDF(const Vector3f& sample);
   
     // Uniformly sample a vector on the unit hemisphere around the pole (0,0,1) with respect to projected solid angles
     static Vector3f SquareToHemisphereCosine(const Vector2f& sample);
   
     static Float SquareToHemisphereCosinePDF(const Vector3f& sample);
   };
   }  // namespace Dibella
