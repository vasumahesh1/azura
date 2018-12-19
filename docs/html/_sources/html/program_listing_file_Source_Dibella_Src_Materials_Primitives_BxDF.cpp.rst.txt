
.. _program_listing_file_Source_Dibella_Src_Materials_Primitives_BxDF.cpp:

Program Listing for File BxDF.cpp
=================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Dibella_Src_Materials_Primitives_BxDF.cpp>` (``Source\Dibella\Src\Materials\Primitives\BxDF.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "BxDF.h"
   #include "../../Core/Constants.h"
   #include "../../Core/Utils.h"
   #include "../../Core/Warper.h"
   
   namespace Dibella {
   BxDF::BxDF(Allocator* allocator, BxDFType type) : AllocationDependent(allocator), mType(type) {}
   
   Color3f BxDF::SampleF(const Vector3f& wo, Vector3f* wi, const Point2f& xi, Float* pdf, BxDFType* sampledType) const {
     *wi = Warper::SquareToHemisphereUniform(xi);
   
     // Flip the WI.Z if we are in the opposite hemisphere.
     if (wo.z < 0.0) {
       wi->z *= -1;
     }
   
     *pdf = CalculatePDF(wo, *wi);
     return CalculateF(wo, *wi);
   }
   
   float BxDF::CalculatePDF(const Vector3f& wo, const Vector3f& wi) const {
     return SameHemisphere(wo, wi) ? INV2_PI : 0;
   }
   
   bool BxDF::IsType(BxDFType t) const {
     return (mType & t) == mType;
   }
   };  // namespace Dibella
