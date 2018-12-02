#pragma once
#include <Types.h>
#include <Containers/Vector.h>
#include <Math/Core.h>
#include <array>

namespace Azura {
namespace Physics {
namespace PBD {

struct ConstraintPoint {
  U32 m_dataIdx;
};

struct ConstraintPointDelta {
  U32 m_dataIdx;
  Vector3f m_delta;
};

class DistanceConstraint {

public:
  DistanceConstraint(const ConstraintPoint& x1,
                     const ConstraintPoint& x2,
                     float restLength);

  void Compute(float stiffness, const Containers::Vector<Vector3f>& currentPoints,
               const Containers::Vector<float>& currentInvMass,
               Containers::Vector<ConstraintPointDelta>& result);

  bool operator<(const DistanceConstraint& rhs) const;

  bool operator==(const DistanceConstraint& rhs) const;

private:
  ConstraintPoint m_points[2]{};
  float m_restLength;
};

class LongRangeConstraint {

public:
  LongRangeConstraint(const ConstraintPoint& x1,
    const ConstraintPoint& x2,
    float restLength);

  void Compute(float stiffness, const Containers::Vector<Vector3f>& currentPoints,
    const Containers::Vector<float>& currentInvMass,
    Containers::Vector<ConstraintPointDelta>& result);

  bool operator<(const LongRangeConstraint& rhs) const;

  bool operator==(const LongRangeConstraint& rhs) const;

private:
  ConstraintPoint m_points[2]{};
  float m_restLength;
};


class BendingConstraint {

public:
  BendingConstraint(const Containers::Vector<Vector3f>& currentPoints,
                    const ConstraintPoint& x0,
                    const ConstraintPoint& x1,
                    const ConstraintPoint& x2,
                    const ConstraintPoint& x3);

  void Compute(float stiffness, const Containers::Vector<Vector3f>& currentPoints,
               const Containers::Vector<float>& currentInvMass,
               Containers::Vector<ConstraintPointDelta>& result);

private:
  static Vector3f ComputeBendingGradient(const Containers::Vector<Vector3f>& currentPositions,
                                  const Matrix4f& Q,
                                  const std::array<U32, 4>& indices,
                                  U32 rowI);

  ConstraintPoint m_points[4]{};
  Matrix4f m_Q{};
};

} // namespace PBD
} // namespace Physics
} // namespace Azura
