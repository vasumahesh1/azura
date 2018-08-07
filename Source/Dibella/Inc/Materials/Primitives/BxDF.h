#pragma once
#include "../../Core/AllocationDependent.h"
#include "../../Core/Types.h"
#include "BxDFType.h"

namespace Dibella {

  class Allocator;

  class BxDF : protected AllocationDependent {
  public:
    explicit BxDF(Allocator* allocator, BxDFType type);
    virtual ~BxDF() = default;

    BxDF(const BxDF& other) = default;
    BxDF(BxDF&& other) noexcept = default;

    BxDF& operator=(const BxDF& other) = delete;
    BxDF& operator=(BxDF&& other) noexcept = delete;

    virtual Color3f CalculateF(const Vector3f& woW, const Vector3f& wiW) const = 0;

    virtual Color3f SampleF(const Vector3f& wo, Vector3f* wi, const Point2f& xi, Float* pdf,
                            BxDFType* sampledType = nullptr) const;

    virtual float CalculatePDF(const Vector3f& wo, const Vector3f& wi) const;

    bool IsType(BxDFType t) const;

  protected:
    const BxDFType mType;
  };
};
