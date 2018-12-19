
.. _program_listing_file_Source_Dibella_Src_Scene_Primitives_Shape.cpp:

Program Listing for File Shape.cpp
==================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Dibella_Src_Scene_Primitives_Shape.cpp>` (``Source\Dibella\Src\Scene\Primitives\Shape.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Shape.h"
   
   #include <cmath>
   #include "../../Core/Constants.h"
   
   namespace Dibella {
   void Shape::InitializeIntersection(Intersection* isect, Float paramT, Point3f pLocal) const {
     ComputeTBN(pLocal, &(isect->mNormalGeometric), &(isect->mTangent), &(isect->mBitangent));
   
     isect->mPoint  = Point3f((mTransform.Get() * Vector4f(pLocal, 1)).xyz());
     isect->mUV     = GetUVCoordinates(pLocal);
     isect->mParamT = paramT;
   }
   
   Intersection Shape::Sample(const Intersection& ref, const Point2f& xi, Float* pdf) const {
     const Intersection isectShape = Sample(xi, pdf);
   
     Vector3f toRef = isectShape.mPoint - ref.mPoint;
   
     const Vector3f wi = toRef.Normalized();
   
     // TODO: [REMOVAL] Might Remove?
     toRef = -toRef;
   
     const Float dist = toRef.LengthSquared();
   
     const Float dot = std::abs(Vector3f::DotProduct(isectShape.mNormalGeometric, -wi));
   
     if (dot <= EPSILON) {
       *pdf = 0;
       return isectShape;
     }
   
     *pdf = dist / (dot / *pdf);
   
     return isectShape;
   }
   }  // namespace Dibella
