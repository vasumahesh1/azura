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

  std::array<Vector4f, 4> Compute(const std::vector<Vector4f>& currentPositions, float stiffness) const;
};

class ClothMesh final : public Math::Geometry {
public:
  ClothMesh(const Vector2f& boundMin , const Vector2f& boundMax);
  ClothMesh(const Vector2f& boundMin , const Vector2f& boundMax, const Vector2u& subDivisions);

  U32 VertexDataSize() const override;
  U32 IndexDataSize() const override;
  const U8* VertexData() const override;
  const U8* IndexData() const override;
  RawStorageFormat GetVertexFormat() const override;
  RawStorageFormat GetIndexFormat() const override;
  U32 GetVertexCount() const override;
  U32 GetIndexCount() const override;
  U32 TotalDataSize() const override;

  const std::vector<Vector4f>& GetVertices() const;
  const std::vector<DistanceConstraint>& GetEdgeConstraints() const;
  const std::vector<BendingConstraint>& GetBendingConstraints() const;
  const std::vector<U32>& GetAnchorIds() const;
  std::vector<Vector4f>& GetVertices();

  const std::vector<Vector3f>& GetNormals() const;
  std::vector<Vector3f>& GetNormals();
  void AddEdgeConstraint(const DistanceConstraint & e);

  U32 NormalDataSize() const override;
  const U8* NormalData() const override;
  RawStorageFormat GetNormalFormat() const override;

private:
  void AddEdgeTriangleNeighbor(const Edge& edge, const U32 triangleIdx);

  std::vector<U32> m_anchorIdx;

  std::unordered_map<Edge, std::vector<U32>, EdgeMapHash> m_edgeTriangleMap;

  std::vector<Vector4f> m_vertices;
  std::vector<Vector3f> m_normals{};
  std::vector<Vector3u> m_triangles;
  std::vector<DistanceConstraint> m_distanceConstraints;
  std::vector<BendingConstraint> m_bendingConstraints;
};

} // namespace Azura
