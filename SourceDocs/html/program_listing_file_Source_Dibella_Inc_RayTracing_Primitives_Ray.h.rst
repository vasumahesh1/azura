
.. _program_listing_file_Source_Dibella_Inc_RayTracing_Primitives_Ray.h:

Program Listing for File Ray.h
==============================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Dibella_Inc_RayTracing_Primitives_Ray.h>` (``Source\Dibella\Inc\RayTracing\Primitives\Ray.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "../../Core/Types.h"
   
   namespace Dibella {
   
   class Ray {
    public:
     Ray(const Point3f &o, const Vector3f &d);
     Ray(const Vector4f &o, const Vector4f &d);
   
     // Return a copy of this ray that has been transformed
     // by the input transformation matrix.
     Ray GetTransformedCopy(const Matrix4f &T) const;
   
    private:
     Point3f mOrigin;
     Vector3f mDirection;
   };
   }  // namespace Dibella
