#pragma once

#include "Math/Core.h"
#include "Math/Geometry.h"

#include <vector>
#include <unordered_map>

#include <boost/container_hash/hash.hpp>

namespace Azura {
  struct Edge
  {
    U32 m_indexA;
    U32 m_indexB;

    bool operator==(const Edge& rhs) const
    {
      return ((m_indexA == rhs.m_indexA) && (m_indexB == rhs.m_indexB)) || ((m_indexA == rhs.m_indexB) && (m_indexB == rhs.m_indexA));
    }
  };

  struct EdgeMapHash {
    std::size_t operator () (const Edge& e) const {
      return boost::hash_value(std::make_pair(e.m_indexA, e.m_indexB)) + boost::hash_value(std::make_pair(e.m_indexB, e.m_indexA));
    }
  };

  struct DistanceConstraint {
    U32 m_indexA;
    U32 m_indexB;
    float m_restLength;
    float m_invMass1{1.0f};
    float m_invMass2{1.0f};

    bool operator<(const DistanceConstraint& rhs) const
    {
      return std::tie(m_indexA, m_indexB) < std::tie(rhs.m_indexA, rhs.m_indexB);
    }

    bool operator==(const DistanceConstraint& rhs) const
    {
      return ((m_indexA == rhs.m_indexA) && (m_indexB == rhs.m_indexB)) || ((m_indexA == rhs.m_indexB) && (m_indexB == rhs.m_indexA));
    }

    Vector4f ComputeDeltaX1(const std::vector<Vector4f>& currentPositions, float k) const
    {
      const float invMassFactor = m_invMass1 / (m_invMass1 + m_invMass2);
      const Vector4f x12 = currentPositions[m_indexA] - currentPositions[m_indexB];
      return (k * -1.0f * invMassFactor * (x12.Length() - m_restLength)) * x12.Normalized();
    }

    Vector4f ComputeDeltaX2(const std::vector<Vector4f>& currentPositions, float k) const
    {
      const float invMassFactor = m_invMass2 / (m_invMass1 + m_invMass2);
      const Vector4f x12 = currentPositions[m_indexA] - currentPositions[m_indexB];
      return (k * invMassFactor * (x12.Length() - m_restLength)) * x12.Normalized();
    }
  };

  struct BendingConstraint {
    U32 m_indexX0;
    U32 m_indexX1;

    U32 m_indexX2;
    U32 m_indexX3;

    float m_invMass0{1.0f};
    float m_invMass1{1.0f};
    float m_invMass2{1.0f};
    float m_invMass3{1.0f};

    Matrix4f m_Q;

    std::array<Vector4f, 4> Compute(const std::vector<Vector4f>& currentPositions, float stiffness) const {
      const std::array<U32, 4> indices = {m_indexX0, m_indexX1, m_indexX2, m_indexX3};
      const std::array<float, 4> invMasses = {m_invMass0, m_invMass1, m_invMass2, m_invMass3};

      float cX = 0.0f;

      for (int idx = 0; idx < 4; ++idx) {
        for (int idy = 0; idy < 4; ++idy) {
          cX += m_Q(idx, idy) * Vector3f::DotProduct(currentPositions[indices[idx]].xyz(), currentPositions[indices[idy]].xyz()); // NOLINT
        }
      }

      cX = cX / 2.0f;

      std::array<Vector3f, 4> partialDeltas = {};
      std::array<Vector4f, 4> finalDeltas = { Vector4f(0.0f), Vector4f(0.0f), Vector4f(0.0f), Vector4f(0.0f) };
      float sum = 0.0f;

      for (U32 idx = 0; idx < 4; ++idx) {
        partialDeltas[idx] = ComputeBendingGradient(currentPositions, m_Q, indices, idx); // NOLINT
        sum += (invMasses[idx] * Vector3f::DotProduct(partialDeltas[idx], partialDeltas[idx])); // NOLINT
      }

      if (std::abs(sum) > EPSILON) {
        for (U32 idx = 0; idx < 4; ++idx) {
          finalDeltas[idx] = Vector4f((stiffness * -1.0f * cX * invMasses[idx] * partialDeltas[idx]) / sum, 0.0f); // NOLINT
        }
      }

      return finalDeltas;
    }

  private:
    Vector3f ComputeBendingGradient(const std::vector<Vector4f>& currentPositions, const Matrix4f& Q, const std::array<U32, 4>& indices, U32 rowI) const
    {
      Vector3f sum = Vector3f(0.0f);
      for (U32 idj = 0; idj < 4; ++idj)
      {
        sum += Q(rowI, idj) * currentPositions[indices[idj]].xyz(); // NOLINT
      }

      return sum;
    }
    
  };
}