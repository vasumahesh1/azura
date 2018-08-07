#include "BSDF.h"

namespace Dibella {
  BSDF::BSDF(const Intersection& isect, Float eta) : mETA(eta) {}

  Color3f BSDF::CalculateF(const Vector3f& woW, const Vector3f& wiW, BxDFType flags) const {}

  Color3f BSDF::SampleF(const Vector3f& woW, Vector3f* wiW, const Point2f& xi, Float* pdf,
                        BxDFType type, BxDFType* sampledType, Float random) const {}

  Float BSDF::CalculatePDF(const Vector3f& woW, const Vector3f& wiW, BxDFType flags) const {}
  int BSDF::BxDFsMatchingFlags(BxDFType flags) const {}
  void BSDF::UpdateTangentSpaceMatrices(const Normal3f& n, const Vector3f& t, const Vector3f b) {}

  void BSDF::Add(BxDF* b) {
    if (mNumBxDF < MAX_BDSF_BXDF) {
      bxdfs[mNumBxDF++] = b;
    }
  }
}
