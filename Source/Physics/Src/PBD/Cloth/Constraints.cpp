#include "Physics/PBD/Cloth/Constraints.h"
#include <array>

namespace Azura {
namespace Physics {
namespace PBD {

namespace {
float ComputeBendingC(const Containers::Vector<Vector3f>& vertices, U32 dest1, U32 dest2, U32 source) {
  const Vector3f v1 = vertices[dest1] - vertices[source];
  const Vector3f v2 = vertices[dest2] - vertices[source];

  return Vector3f::DotProduct(v1, v2) / Vector3f::CrossProduct(v1, v2).Length();
}
} // namespace

DistanceConstraint::DistanceConstraint(const ConstraintPoint& x1,
                                       const ConstraintPoint& x2,
                                       float restLength)
  : m_restLength(restLength) {
  m_points[0] = x1;
  m_points[1] = x2;
}

void DistanceConstraint::Compute(float stiffness,
                                 const Containers::Vector<Vector3f>& currentPoints,
                                 const Containers::Vector<float>& currentInvMass,
                                 Containers::Vector<ConstraintPointDelta>& result) {

  const U32 indexA = m_points[0].m_dataIdx;
  const U32 indexB = m_points[1].m_dataIdx;

  const float invMassSum     = currentInvMass[indexA] + currentInvMass[indexB];
  const float invMassFactor1 = currentInvMass[indexA] / invMassSum;
  const float invMassFactor2 = currentInvMass[indexB] / invMassSum;

  const Vector3f x12 = currentPoints[indexA] - currentPoints[indexB];

  result[0] = ConstraintPointDelta{
    indexA, ((stiffness * -1.0f * invMassFactor1 * (x12.Length() - m_restLength)) * x12.Normalized())
  };
  result[1] = ConstraintPointDelta{
    indexB, ((stiffness * invMassFactor2 * (x12.Length() - m_restLength)) * x12.Normalized())
  };
}

bool DistanceConstraint::operator<(const DistanceConstraint& rhs) const {
  return std::tie(m_points[0].m_dataIdx, m_points[1].m_dataIdx) < std::tie(rhs.m_points[0].m_dataIdx,
                                                                           rhs.m_points[1].m_dataIdx);
}

bool DistanceConstraint::operator==(const DistanceConstraint& rhs) const {
  const U32 indexA = m_points[0].m_dataIdx;
  const U32 indexB = m_points[0].m_dataIdx;

  const U32 rhsIndexA = rhs.m_points[0].m_dataIdx;
  const U32 rhsIndexB = rhs.m_points[0].m_dataIdx;

  return ((indexA == rhsIndexA) && (indexB == rhsIndexB)) || ((indexA == rhsIndexB) && (indexB == rhsIndexA));
}

BendingConstraint::BendingConstraint(const Containers::Vector<Vector3f>& currentPoints,
                                     const ConstraintPoint& x0,
                                     const ConstraintPoint& x1,
                                     const ConstraintPoint& x2,
                                     const ConstraintPoint& x3) {

  m_points[0] = x0;
  m_points[1] = x1;
  m_points[2] = x2;
  m_points[3] = x3;

  const float c01 = ComputeBendingC(currentPoints, x2.m_dataIdx, x0.m_dataIdx, x1.m_dataIdx);
  const float c04 = ComputeBendingC(currentPoints, x3.m_dataIdx, x0.m_dataIdx, x1.m_dataIdx);
  const float c03 = ComputeBendingC(currentPoints, x3.m_dataIdx, x1.m_dataIdx, x0.m_dataIdx);
  const float c02 = ComputeBendingC(currentPoints, x2.m_dataIdx, x1.m_dataIdx, x0.m_dataIdx);

  const float aValue = c01 + c04;
  const float bValue = c02 + c03;
  const float cValue = -c01 - c02;
  const float dValue = -c03 - c04;

  // Triangle 0
  const Vector3f s1 = currentPoints[x2.m_dataIdx] - currentPoints[x1.m_dataIdx];
  const Vector3f s2 = currentPoints[x0.m_dataIdx] - currentPoints[x1.m_dataIdx];
  const float Area0 = Vector3f::CrossProduct(s1, s2).Length() / 2.0f;

  // Triangle 1
  const Vector3f s3 = currentPoints[x3.m_dataIdx] - currentPoints[x1.m_dataIdx];
  const Vector3f s4 = currentPoints[x0.m_dataIdx] - currentPoints[x1.m_dataIdx];
  const float Area1 = Vector3f::CrossProduct(s3, s4).Length() / 2.0f;

  m_Q              = Matrix4f(0.0f);
  m_Q.GetColumn(0) = Vector4f(aValue * aValue, bValue * aValue, cValue * aValue, dValue * aValue);
  m_Q.GetColumn(1) = Vector4f(aValue * bValue, bValue * bValue, cValue * bValue, dValue * bValue);
  m_Q.GetColumn(2) = Vector4f(aValue * cValue, bValue * cValue, cValue * cValue, dValue * cValue);
  m_Q.GetColumn(3) = Vector4f(aValue * dValue, bValue * dValue, cValue * dValue, dValue * dValue);

  m_Q = (3.0f / (Area0 + Area1)) * m_Q;
}

void BendingConstraint::Compute(float stiffness,
                                const Containers::Vector<Vector3f>& currentPoints,
                                const Containers::Vector<float>& currentInvMass,
                                Containers::Vector<ConstraintPointDelta>& result) {

  std::array<U32, 4> indices = {
    m_points[0].m_dataIdx, m_points[1].m_dataIdx, m_points[2].m_dataIdx, m_points[3].m_dataIdx
  };
  const std::array<float, 4> invMasses = {
    currentInvMass[indices[0]], currentInvMass[indices[1]], currentInvMass[indices[2]], currentInvMass[indices[3]]
  };

  float cX = 0.0f;

  for (int idx   = 0; idx < 4; ++idx) {
    for (int idy = 0; idy < 4; ++idy) {
      cX += m_Q(idx, idy) * Vector3f::DotProduct(currentPoints[indices[idx]], currentPoints[indices[idy]]); // NOLINT
    }
  }

  cX = cX / 2.0f;

  std::array<Vector3f, 4> partialDeltas = {};
  float sum                             = 0.0f;

  for (U32 idx         = 0; idx < 4; ++idx) {
    partialDeltas[idx] = ComputeBendingGradient(currentPoints, m_Q, indices, idx);          // NOLINT
    sum += (invMasses[idx] * Vector3f::DotProduct(partialDeltas[idx], partialDeltas[idx])); // NOLINT
  }

  if (std::abs(sum) > EPSILON) {
    for (U32 idx  = 0; idx < 4; ++idx) {
      result[idx] = ConstraintPointDelta{
        indices[idx], ((stiffness * -1.0f * cX * invMasses[idx] * partialDeltas[idx]) / sum)
      };
    }
  }
}

Vector3f BendingConstraint::ComputeBendingGradient(const Containers::Vector<Vector3f>& currentPositions,
                                                   const Matrix4f& Q,
                                                   const std::array<U32, 4>& indices,
                                                   U32 rowI) {
  Vector3f sum = Vector3f(0.0f);
  for (U32 idj = 0; idj < 4; ++idj) {
    sum += Q(rowI, idj) * currentPositions[indices[idj]];
  }

  return sum;
}

} // namespace PBD
} // namespace Physics
} // namespace Azura
