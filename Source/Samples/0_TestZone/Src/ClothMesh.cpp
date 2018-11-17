#include "TestZone/ClothMesh.h"

namespace Azura {

namespace {
const RawStorageFormat VERTEX_FORMAT = RawStorageFormat::R32G32B32A32_FLOAT;
const RawStorageFormat NORMAL_FORMAT = RawStorageFormat::R32G32B32_FLOAT;
const RawStorageFormat INDEX_FORMAT  = RawStorageFormat::R32_UINT;

float ComputeBendingC(const std::vector<Vector4f>& vertices, U32 dest1, U32 dest2, U32 source)
{
  Vector3f v1 = vertices[dest1].xyz() - vertices[source].xyz();
  Vector3f v2 = vertices[dest2].xyz() - vertices[source].xyz();

  return Vector3f::DotProduct(v1, v2) / Vector3f::CrossProduct(v1, v2).Length();
}

Vector3f ComputeBendingGradient(const std::vector<Vector4f>& currentPositions, const Matrix4f& Q, const std::array<U32, 4>& indices, U32 rowI)
{
  Vector3f sum = Vector3f(0.0f);
  for (U32 idj = 0 ; idj < 4; ++idj)
  {
    sum += Q(rowI, idj) * currentPositions[indices[idj]].xyz();
  }

  return sum;
}

} // namespace

ClothMesh::ClothMesh(const Vector2f& boundMin, const Vector2f& boundMax)
  : ClothMesh(boundMin, boundMax, Vector2u(1, 1)) {
}

ClothMesh::ClothMesh(const Vector2f& boundMin, const Vector2f& boundMax, const Vector2u& subDivisions) {
  const float stepX = float(boundMax[0] - boundMin[0]) / subDivisions[0];
  const float stepY = float(boundMax[1] - boundMin[1]) / subDivisions[1];

  const U32 ANCHOR_IDX_1 = 0;
  const U32 ANCHOR_IDX_2 = (subDivisions[0] * (subDivisions[1] + 1));

  m_anchorIdx.reserve(2);
  m_anchorIdx.push_back(ANCHOR_IDX_1);
  m_anchorIdx.push_back(ANCHOR_IDX_2);

  for (float xCoord   = boundMin[0]; xCoord <= boundMax[0] + EPSILON;) {
    for (float yCoord = boundMin[1]; yCoord <= boundMax[1] + EPSILON;) {
      m_vertices.emplace_back(Vector4f(xCoord, 0, yCoord, 1));
      m_normals.emplace_back(Vector3f(0, 1, 0));
      yCoord += stepY;
    }

    xCoord += stepX;
  }

  for (U32 idx     = 0; idx < subDivisions[0]; ++idx) {
    for (U32 idy   = 0; idy < subDivisions[1]; ++idy) {
      const U32 i1 = ((subDivisions[1] + 1) * idx) + idy;
      const U32 i2 = ((subDivisions[1] + 1) * (idx + 1)) + idy;
      const U32 i21 = i2 + 1;
      const U32 i11 = i1 + 1;

      // Anchors
      float invMass1 = 1.0f;
      if (i1 == ANCHOR_IDX_1 || i1 == ANCHOR_IDX_2)
      {
        invMass1 = 0;
      }

      float invMass2 = 1.0f;
      if (i2 == ANCHOR_IDX_1 || i2 == ANCHOR_IDX_2)
      {
        invMass2 = 0;
      }

      float invMass3 = 1.0f;
      if (i21 == ANCHOR_IDX_1 || i21 == ANCHOR_IDX_2)
      {
        invMass3 = 0;
      }

      float invMass4 = 1.0f;
      if (i11 == ANCHOR_IDX_1 || i11 == ANCHOR_IDX_2)
      {
        invMass4 = 0;
      }

      const bool isEvenX = idx % 2 == 0;
      const bool isEvenY = idy % 2 == 0;

      const U32 triangleIdx = m_triangles.size();

      if ((isEvenX && !isEvenY) || (!isEvenX && isEvenY))
      {
        m_triangles.emplace_back(Vector3u(i1, i2, i21));
        m_triangles.emplace_back(Vector3u(i1, i21, i11));

        AddEdgeTriangleNeighbor(Edge{ i1, i2 }, triangleIdx);
        AddEdgeTriangleNeighbor(Edge{ i2, i21 }, triangleIdx);
        AddEdgeTriangleNeighbor(Edge{ i21, i1 }, triangleIdx);

        AddEdgeTriangleNeighbor(Edge{ i1, i21 }, triangleIdx + 1);
        AddEdgeTriangleNeighbor(Edge{ i21, i11 }, triangleIdx + 1);
        AddEdgeTriangleNeighbor(Edge{ i11, i1 }, triangleIdx + 1);

        // Distance Constraints
        DistanceConstraint e1 = { i1, i2, (m_vertices[i1] - m_vertices[i2]).Length(), invMass1, invMass2 };
        DistanceConstraint e2 = { i2, i21, (m_vertices[i2] - m_vertices[i21]).Length(), invMass2, invMass3 };
        DistanceConstraint e3 = { i21, i1, (m_vertices[i21] - m_vertices[i1]).Length(), invMass3, invMass1 };

        DistanceConstraint e4 = { i1, i21, (m_vertices[i1] - m_vertices[i21]).Length(), invMass1, invMass3 };
        DistanceConstraint e5 = { i21, i11, (m_vertices[i21] - m_vertices[i11]).Length(), invMass3, invMass4 };
        DistanceConstraint e6 = { i11, i1, (m_vertices[i11] - m_vertices[i1]).Length(), invMass4, invMass1 };

        AddEdgeConstraint(e1);
        AddEdgeConstraint(e2);
        AddEdgeConstraint(e3);
        AddEdgeConstraint(e4);
        AddEdgeConstraint(e5);
        AddEdgeConstraint(e6);
      }
      else
      {
        m_triangles.emplace_back(Vector3u(i1, i2, i11));
        m_triangles.emplace_back(Vector3u(i2, i21, i11));

        AddEdgeTriangleNeighbor(Edge{ i1, i2 }, triangleIdx);
        AddEdgeTriangleNeighbor(Edge{ i2, i11 }, triangleIdx);
        AddEdgeTriangleNeighbor(Edge{ i11, i1 }, triangleIdx);

        AddEdgeTriangleNeighbor(Edge{ i2, i21 }, triangleIdx + 1);
        AddEdgeTriangleNeighbor(Edge{ i21, i11 }, triangleIdx + 1);
        AddEdgeTriangleNeighbor(Edge{ i11, i2 }, triangleIdx + 1);

        // Distance Constraints
        DistanceConstraint e1 = { i1, i2, (m_vertices[i1] - m_vertices[i2]).Length(), invMass1, invMass2 };
        DistanceConstraint e2 = { i2, i11, (m_vertices[i2] - m_vertices[i11]).Length(), invMass2, invMass4 };
        DistanceConstraint e3 = { i11, i1, (m_vertices[i11] - m_vertices[i1]).Length(), invMass4, invMass1 };

        DistanceConstraint e4 = { i2, i21, (m_vertices[i2] - m_vertices[i21]).Length(), invMass2, invMass3 };
        DistanceConstraint e5 = { i21, i11, (m_vertices[i21] - m_vertices[i11]).Length(), invMass3, invMass4 };
        DistanceConstraint e6 = { i11, i2, (m_vertices[i11] - m_vertices[i2]).Length(), invMass4, invMass2 };

        AddEdgeConstraint(e1);
        AddEdgeConstraint(e2);
        AddEdgeConstraint(e3);
        AddEdgeConstraint(e4);
        AddEdgeConstraint(e5);
        AddEdgeConstraint(e6);
      }
    }
  }

  for(const auto& pair : m_edgeTriangleMap)
  {
    if (pair.second.size() != 2)
    {
      continue;
    }

    BendingConstraint constraint = {};

    const Edge& edge = pair.first;

    constraint.m_indexX0 = edge.m_indexA;
    constraint.m_indexX1 = edge.m_indexB;

    Vector3u tri1 = m_triangles[pair.second[0]];
    Vector3u tri2 = m_triangles[pair.second[1]];

    for(U32 idx = 0; idx < 3; ++idx)
    {
      if (tri1[idx] == edge.m_indexA)
      {
        continue;
      }

      if (tri1[idx] == edge.m_indexB)
      {
        continue;
      }

      constraint.m_indexX2 = tri1[idx];
    }

    for(U32 idx = 0; idx < 3; ++idx)
    {
      if (tri2[idx] == edge.m_indexA)
      {
        continue;
      }

      if (tri2[idx] == edge.m_indexB)
      {
        continue;
      }

      constraint.m_indexX3 = tri2[idx];
    }

    const float c01 = ComputeBendingC(m_vertices, constraint.m_indexX2, constraint.m_indexX0, constraint.m_indexX1);
    const float c04 = ComputeBendingC(m_vertices, constraint.m_indexX3, constraint.m_indexX0, constraint.m_indexX1);
    const float c03 = ComputeBendingC(m_vertices, constraint.m_indexX3, constraint.m_indexX1, constraint.m_indexX0);
    const float c02 = ComputeBendingC(m_vertices, constraint.m_indexX2, constraint.m_indexX1, constraint.m_indexX0);

    const float aValue = c01 + c04;
    const float bValue = c02 + c03;
    const float cValue = -c01 - c02;
    const float dValue = -c03 - c04;

    // Triangle 0
    Vector3f s1 = m_vertices[constraint.m_indexX2].xyz() - m_vertices[constraint.m_indexX1].xyz();
    Vector3f s2 = m_vertices[constraint.m_indexX0].xyz() - m_vertices[constraint.m_indexX1].xyz();
    const float Area0 = Vector3f::CrossProduct(s1, s2).Length() / 2.0f;

    // Triangle 1
    Vector3f s3 = m_vertices[constraint.m_indexX3].xyz() - m_vertices[constraint.m_indexX1].xyz();
    Vector3f s4 = m_vertices[constraint.m_indexX0].xyz() - m_vertices[constraint.m_indexX1].xyz();
    const float Area1 = Vector3f::CrossProduct(s3, s4).Length() / 2.0f;

    constraint.m_Q = Matrix4f(0.0f);
    constraint.m_Q.GetColumn(0) = Vector4f(aValue * aValue, bValue * aValue, cValue * aValue, dValue * aValue);
    constraint.m_Q.GetColumn(1) = Vector4f(aValue * bValue, bValue * bValue, cValue * bValue, dValue * bValue);
    constraint.m_Q.GetColumn(2) = Vector4f(aValue * cValue, bValue * cValue, cValue * cValue, dValue * cValue);
    constraint.m_Q.GetColumn(3) = Vector4f(aValue * dValue, bValue * dValue, cValue * dValue, dValue * dValue);

    constraint.m_Q = (3.0f / (Area0 + Area1)) * constraint.m_Q;

    if (constraint.m_indexX0 == ANCHOR_IDX_1 || constraint.m_indexX0 == ANCHOR_IDX_2)
    {
      constraint.m_invMass0 = 0;
    }

    if (constraint.m_indexX1 == ANCHOR_IDX_1 || constraint.m_indexX1 == ANCHOR_IDX_2)
    {
      constraint.m_invMass1 = 0;
    }

    if (constraint.m_indexX2 == ANCHOR_IDX_1 || constraint.m_indexX2 == ANCHOR_IDX_2)
    {
      constraint.m_invMass2 = 0;
    }

    if (constraint.m_indexX3 == ANCHOR_IDX_1 || constraint.m_indexX3 == ANCHOR_IDX_2)
    {
      constraint.m_invMass3 = 0;
    }

    m_bendingConstraints.push_back(constraint);
  }
}

std::array<Vector4f, 4> BendingConstraint::Compute(const std::vector<Vector4f>& currentPositions, float stiffness) const {
  const std::array<U32, 4> indices = {m_indexX0, m_indexX1, m_indexX2, m_indexX3};
  const std::array<float, 4> invMasses = {m_invMass0, m_invMass1, m_invMass2, m_invMass3};

  float cX = 0.0f;

  for (int idx = 0; idx < 4; ++idx) {
    for (int idy = 0; idy <= idx; ++idy) {
      cX += m_Q(idx, idy) * Vector3f::DotProduct(currentPositions[indices[idx]].xyz(), currentPositions[indices[idy]].xyz());
    }
  }

  std::array<Vector3f, 4> partialDeltas = {};
  std::array<Vector4f, 4> finalDeltas = { Vector4f(0.0f), Vector4f(0.0f), Vector4f(0.0f), Vector4f(0.0f) };
  float sum = 0.0f;

  for (U32 idx = 0; idx < 4; ++idx) {
    partialDeltas[idx] = ComputeBendingGradient(currentPositions, m_Q, indices, idx);
    sum += (invMasses[idx] * Vector3f::DotProduct(partialDeltas[idx], partialDeltas[idx]));
  }

  if (std::abs(sum) > EPSILON) {
    for (U32 idx = 0; idx < 4; ++idx) {
      finalDeltas[idx] = Vector4f((stiffness * -1.0f * cX * invMasses[idx] * partialDeltas[idx]) / sum, 0.0f);
    }
  }

  return finalDeltas;
}

U32 ClothMesh::VertexDataSize() const {
  return U32(m_vertices.size() * GetFormatSize(VERTEX_FORMAT));
}

U32 ClothMesh::IndexDataSize() const {
  return U32(m_triangles.size() * GetFormatSize(INDEX_FORMAT) * 3);
}

U32 ClothMesh::NormalDataSize() const {
  return U32(m_normals.size() * GetFormatSize(NORMAL_FORMAT));
}

const U8* ClothMesh::VertexData() const {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<const U8*>(m_vertices.data());
}

const U8* ClothMesh::IndexData() const {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<const U8*>(m_triangles.data());
}

const U8* ClothMesh::NormalData() const {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<const U8*>(m_normals.data());
}

RawStorageFormat ClothMesh::GetVertexFormat() const {
  return VERTEX_FORMAT;
}

RawStorageFormat ClothMesh::GetIndexFormat() const {
  return INDEX_FORMAT;
}

RawStorageFormat ClothMesh::GetNormalFormat() const {
  return NORMAL_FORMAT;
}

U32 ClothMesh::GetVertexCount() const {
  return U32(m_vertices.size());
}

U32 ClothMesh::GetIndexCount() const {
  return U32(m_triangles.size() * 3);
}

U32 ClothMesh::TotalDataSize() const {
  return VertexDataSize() + IndexDataSize() + NormalDataSize();
}

const std::vector<Vector4f>& ClothMesh::GetVertices() const {
  return m_vertices;
}

const std::vector<DistanceConstraint>& ClothMesh::GetEdgeConstraints() const {
  return m_distanceConstraints;
}

const std::vector<BendingConstraint>& ClothMesh::GetBendingConstraints() const {
  return m_bendingConstraints;
}

const std::vector<U32>& ClothMesh::GetAnchorIds() const {
  return m_anchorIdx;
}

std::vector<Vector4f>& ClothMesh::GetVertices() {
  return m_vertices;
}

const std::vector<Vector3f>& ClothMesh::GetNormals() const {
  return m_normals;
}

std::vector<Vector3f>& ClothMesh::GetNormals() {
  return m_normals;
}

void ClothMesh::AddEdgeTriangleNeighbor(const Edge& edge, const U32 triangleIdx) {
  const auto itr = m_edgeTriangleMap.find(edge);
  
  if (itr == m_edgeTriangleMap.end())
  {
    m_edgeTriangleMap[edge] = std::vector<U32>();
    m_edgeTriangleMap[edge].reserve(2);
  }

  m_edgeTriangleMap[edge].push_back(triangleIdx);
}

void ClothMesh::AddEdgeConstraint(const DistanceConstraint& e) {
  for (const auto& edge : m_distanceConstraints)
  {
    if (edge == e)
    {
      return;
    }
  }

  m_distanceConstraints.push_back(e);
}

} // namespace Azura
