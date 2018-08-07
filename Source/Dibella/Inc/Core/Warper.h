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

    /**
     * \brief Uniformly sample a vector on a spherical cap around (0, 0, 1)
     *
     * A spherical cap is the subset of a unit sphere whose directions
     * make an angle of less than 'theta' with the north pole. This function
     * expects the minimum angle 'theta' as a parameter.
     */
    static Vector3f SquareToSphereCapUniform(const Vector2f& sample, Float thetaMin);

    static Float SquareToSphereCapUniformPDF(const Vector3f& sample, Float thetaMin);

    // Uniformly sample a vector on the unit hemisphere around the pole (0,0,1) with respect to solid angles
    static Vector3f SquareToHemisphereUniform(const Vector2f& sample);

    static Float SquareToHemisphereUniformPDF(const Vector3f& sample);

    // Uniformly sample a vector on the unit hemisphere around the pole (0,0,1) with respect to projected solid angles
    static Vector3f SquareToHemisphereCosine(const Vector2f& sample);

    static Float SquareToHemisphereCosinePDF(const Vector3f& sample);
  };
}
