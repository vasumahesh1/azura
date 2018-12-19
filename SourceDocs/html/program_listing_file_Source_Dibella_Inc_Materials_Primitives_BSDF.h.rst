
.. _program_listing_file_Source_Dibella_Inc_Materials_Primitives_BSDF.h:

Program Listing for File BSDF.h
===============================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Dibella_Inc_Materials_Primitives_BSDF.h>` (``Source\Dibella\Inc\Materials\Primitives\BSDF.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "../../Core/Constants.h"
   #include "BxDF.h"
   
   namespace Dibella {
   class Intersection;
   
   class BSDF {
    public:
     BSDF(const Intersection& isect, Float eta = 1);
   
     void Add(BxDF* b);
   
     Color3f CalculateF(const Vector3f& woW, const Vector3f& wiW, BxDFType flags = BxDFType::BsdfAll) const;
   
     Color3f SampleF(const Vector3f& woW,
                     Vector3f* wiW,
                     const Point2f& xi,
                     Float* pdf,
                     BxDFType type         = BxDFType::BsdfAll,
                     BxDFType* sampledType = nullptr,
                     Float random          = 0.0f) const;
   
     Float CalculatePDF(const Vector3f& woW, const Vector3f& wiW, BxDFType flags = BxDFType::BsdfAll) const;
   
     int BxDFsMatchingFlags(BxDFType flags) const;
   
     static void UpdateTangentSpaceMatrices(const Normal3f& n, const Vector3f& t, const Vector3f b);
   
    protected:
     // Transforms rays from world space into tangent space,
     // where the surface normal is always treated as (0, 0, 1)
     Matrix3f mWorldToTangent;
   
     // Transforms rays from tangent space into world space.
     // This is the inverse of mWorldToTangent (incidentally, inverse(mWorldToTangent) = transpose(mWorldToTangent))
     Matrix3f mTangentToWorld;
   
     // May be the geometric normal OR the shading normal at the point of intersection.
     // If the Material that created this BSDF had a normal map, then this will be the latter.
     Normal3f mNormal;
   
     // The ratio of indices of refraction at this surface point. Irrelevant for opaque surfaces.
     Float mETA;
   
     // Number of active BxDFs
     int mNumBxDF{0};
   
     // The collection of BxDFs contained in this BSDF
     BxDF* bxdfs[MAX_BDSF_BXDF];
   };
   }  // namespace Dibella
