#pragma once
#include "../../Core/Types.h"
#include "Ray.h"


namespace Dibella {
  class BSDF;
  class SceneObject;
  class Shape;

  class Intersection {
    friend class SceneObject;
    friend class Shape;
  public:
    Intersection();

    // Ask _objectHit_ to produce a BSDF
    // based on other data stored in this
    // Intersection, e.g. the surface normal
    // and UV coordinates
    bool ProduceBSDF();

    // Returns the light emitted from this Intersection
    // along _wo_, provided that this Intersection is
    // on the surface of an AreaLight. If not, then
    // zero light is returned.
    Color3f Le(const Vector3f& wo) const;

    // Instantiate a Ray that originates from this Intersection and
    // travels in mDirection d.
    Ray SpawnRay(const Vector3f& d) const;

  protected:
    // The place at which the intersection occurred
    Point3f mPoint;

    // The surface normal at the point of intersection, NO alterations like normal mapping applied
    Normal3f mNormalGeometric;
    Normal3f mNormalShading;

    // The UV coordinates computed at the intersection
    Point2f mUV;

    // The parameterization for the ray (in world space) that generated this intersection.
    // t is equal to the distance from the point of intersection to the ray's mOrigin if the ray's mDirection is normalized.
    float mParamT;

    // The object that the ray intersected, or nullptr if the ray hit nothing.
    const SceneObject* mTargetHit;

    // The Bidirection Scattering Distribution Function found at the intersection.
    BSDF* mBSDF;

    // World-space vectors that form an orthonormal basis with the surface normal.
    Vector3f mTangent;
    Vector3f mBitangent;
  };
}
