#pragma once
#include "../../Core/Types.h"
#include "../../RayTracing/Primitives/Intersection.h"
#include "Transform.h"

namespace Dibella {
  class Shape {

  public:
    virtual ~Shape() = default;

    Shape(const Shape& other) = default;
    Shape(Shape&& other) = default;
    Shape& operator=(const Shape& other) = default;
    Shape& operator=(Shape&& other) noexcept = default;

    // Find the intersection of the ray with this Shape
    virtual bool Intersect(const Ray& ray, Intersection* isect) const = 0;
    virtual Point2f GetUVCoordinates(const Point3f& point) const = 0;

    // Given a t parameterization of the Ray, compute all requisite intersection information
    virtual void InitializeIntersection(Intersection* isect, Float paramT, Point3f pLocal) const;

    // A helper function for computing the world-space normal, tangent, and bitangent at local-space point P
    virtual void ComputeTBN(
      const Point3f& point,
      Normal3f* nor,
      Vector3f* tan,
      Vector3f* bit
    ) const = 0;

    // Compute and return the surface area of the shape in world space
    // i.e. you must account for the Transform applied to the Shape
    virtual Float Area() const = 0;

    // Invokes the Sample below and converts the PDF from an area measure
    // to a solid angle measure
    virtual Intersection Sample(const Intersection& ref, const Point2f& xi,
                                Float* pdf) const;

    // Sample a point on the surface of the shape and return the PDF with
    // respect to area on the surface.
    virtual Intersection Sample(const Point2f& xi, Float* pdf) const = 0;

  protected:
    Transform mTransform;
  };
}
