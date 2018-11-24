#include "Physics/Geometry/ClothPlane.h"

#include "Physics/Geometry/GenericTypes.h"

namespace Azura {
namespace Physics {

using namespace PBD; // NOLINT

namespace {
const RawStorageFormat VERTEX_FORMAT = RawStorageFormat::R32G32B32_FLOAT;
const RawStorageFormat NORMAL_FORMAT = RawStorageFormat::R32G32B32_FLOAT;
const RawStorageFormat INDEX_FORMAT  = RawStorageFormat::R32_UINT;

float ComputeBendingC(const std::vector<Vector4f>& vertices, U32 dest1, U32 dest2, U32 source) {
  Vector3f v1 = vertices[dest1].xyz() - vertices[source].xyz();
  Vector3f v2 = vertices[dest2].xyz() - vertices[source].xyz();

  return Vector3f::DotProduct(v1, v2) / Vector3f::CrossProduct(v1, v2).Length();
}

} // namespace

ClothPlane::ClothPlane(const Vector2f& boundMin, const Vector2f& boundMax, Memory::Allocator& allocator)
  : ClothPlane(ClothTriangulation::Regular, boundMin, boundMax, Vector2u(1, 1), allocator) {
}

ClothPlane::ClothPlane(ClothTriangulation triangulation, 
                       const Vector2f& boundMin,
                       const Vector2f& boundMax,
                       const Vector2u& subDivisions,
                       Memory::Allocator& allocator)
  : m_vertices(allocator),
    m_vertexInvMass(allocator),
    m_normals(allocator),
    m_triangles(allocator) {
  const float stepX = float(boundMax[0] - boundMin[0]) / subDivisions[0];
  const float stepY = float(boundMax[1] - boundMin[1]) / subDivisions[1];

  const U32 ANCHOR_IDX_1 = 0;
  const U32 ANCHOR_IDX_2 = (subDivisions[0] * (subDivisions[1] + 1));

  const U32 totalVertices = (subDivisions[0] + 1) * (subDivisions[1] + 1);

  m_anchorIdx.reserve(2);
  m_anchorIdx.push_back(ANCHOR_IDX_1);
  m_anchorIdx.push_back(ANCHOR_IDX_2);

  m_vertices.Reserve(totalVertices);
  m_normals.Reserve(totalVertices);
  m_triangles.Reserve(subDivisions[0] * subDivisions[1] * 2);

  for (float xCoord   = boundMin[0]; xCoord <= boundMax[0] + EPSILON;) {
    for (float yCoord = boundMin[1]; yCoord <= boundMax[1] + EPSILON;) {
      m_vertices.EmplaceBack(xCoord, 0.0f, yCoord);
      m_normals.EmplaceBack(0.0f, 1.0f, 0.0f);
      yCoord += stepY;
    }

    xCoord += stepX;
  }

  m_vertexInvMass.Reserve(m_vertices.GetSize());
  for (SizeType idx = 0; idx < m_vertices.GetSize(); ++idx) {
    if (std::find(m_anchorIdx.begin(), m_anchorIdx.end(), idx) != m_anchorIdx.end()) {
      m_vertexInvMass.PushBack(0.0f);
      continue;
    }

    m_vertexInvMass.PushBack(1.0f);
  }

  for (U32 idx      = 0; idx < subDivisions[0]; ++idx) {
    for (U32 idy    = 0; idy < subDivisions[1]; ++idy) {
      const U32 i1  = ((subDivisions[1] + 1) * idx) + idy;
      const U32 i2  = ((subDivisions[1] + 1) * (idx + 1)) + idy;
      const U32 i21 = i2 + 1;
      const U32 i11 = i1 + 1;

      // Anchors
      const bool isEvenX = idx % 2 == 0;
      const bool isEvenY = idy % 2 == 0;

      const auto triangleIdx = U32(m_triangles.GetSize());

      if (triangulation == ClothTriangulation::Regular || ((isEvenX && !isEvenY) || (!isEvenX && isEvenY))) {
        m_triangles.EmplaceBack(i1, i2, i21);
        m_triangles.EmplaceBack(i1, i21, i11);

        AddEdgeTriangleNeighbor(Edge{i1, i2}, triangleIdx);
        AddEdgeTriangleNeighbor(Edge{i2, i21}, triangleIdx);
        AddEdgeTriangleNeighbor(Edge{i21, i1}, triangleIdx);

        AddEdgeTriangleNeighbor(Edge{i1, i21}, triangleIdx + 1);
        AddEdgeTriangleNeighbor(Edge{i21, i11}, triangleIdx + 1);
        AddEdgeTriangleNeighbor(Edge{i11, i1}, triangleIdx + 1);
      } else {
        m_triangles.EmplaceBack(Vector3u(i1, i2, i11));
        m_triangles.EmplaceBack(Vector3u(i2, i21, i11));

        AddEdgeTriangleNeighbor(Edge{i1, i2}, triangleIdx);
        AddEdgeTriangleNeighbor(Edge{i2, i11}, triangleIdx);
        AddEdgeTriangleNeighbor(Edge{i11, i1}, triangleIdx);

        AddEdgeTriangleNeighbor(Edge{i2, i21}, triangleIdx + 1);
        AddEdgeTriangleNeighbor(Edge{i21, i11}, triangleIdx + 1);
        AddEdgeTriangleNeighbor(Edge{i11, i2}, triangleIdx + 1);
      }
    }
  }
}

U32 ClothPlane::VertexDataSize() const {
  return U32(m_vertices.GetSize() * GetFormatSize(VERTEX_FORMAT));
}

U32 ClothPlane::IndexDataSize() const {
  return U32(m_triangles.GetSize() * GetFormatSize(INDEX_FORMAT) * 3);
}

U32 ClothPlane::NormalDataSize() const {
  return U32(m_normals.GetSize() * GetFormatSize(NORMAL_FORMAT));
}

const U8* ClothPlane::VertexData() const {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<const U8*>(m_vertices.Data());
}

const U8* ClothPlane::IndexData() const {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<const U8*>(m_triangles.Data());
}

const U8* ClothPlane::NormalData() const {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<const U8*>(m_normals.Data());
}

RawStorageFormat ClothPlane::GetVertexFormat() const {
  return VERTEX_FORMAT;
}

RawStorageFormat ClothPlane::GetIndexFormat() const {
  return INDEX_FORMAT;
}

RawStorageFormat ClothPlane::GetNormalFormat() const {
  return NORMAL_FORMAT;
}

const Containers::Vector<float>& ClothPlane::GetVertexInverseMass() const {
  return m_vertexInvMass;
}

ClothSolvingView ClothPlane::GetPBDSolvingView(Memory::Allocator& allocator) {

  U32 numBendingConstraints = 0;
  for (const auto& pair : m_edgeTriangleMap) {
    if (pair.second.size() != 2) {
      continue;
    }

    ++numBendingConstraints;
  }

  ClothSolvingView solvingView = ClothSolvingView(
    m_vertices,
    m_vertexInvMass,
    U32(m_edgeTriangleMap.size()),
    numBendingConstraints,
    allocator
  );

  for (const auto& pair : m_edgeTriangleMap) {
    const Edge& edge = pair.first;

    // Add Distance Constraint
    solvingView.AddConstraint({
      ConstraintPoint{edge.m_indexA},
      ConstraintPoint{edge.m_indexB},
      (m_vertices[edge.m_indexA] - m_vertices[edge.m_indexB]).Length()
    });

    // Add Bending Constraint
    if (pair.second.size() != 2) {
      continue;
    }

    const U32 indexX0 = edge.m_indexA;
    const U32 indexX1 = edge.m_indexB;
    U32 indexX2       = 0;
    U32 indexX3       = 0;

    Vector3u tri1 = m_triangles[pair.second[0]];
    Vector3u tri2 = m_triangles[pair.second[1]];

    for (U32 idx = 0; idx < 3; ++idx) {
      if (tri1[idx] == edge.m_indexA) {
        continue;
      }

      if (tri1[idx] == edge.m_indexB) {
        continue;
      }

      indexX2 = tri1[idx];
    }

    for (U32 idx = 0; idx < 3; ++idx) {
      if (tri2[idx] == edge.m_indexA) {
        continue;
      }

      if (tri2[idx] == edge.m_indexB) {
        continue;
      }

      indexX3 = tri2[idx];
    }

    solvingView.AddConstraint(BendingConstraint(
      m_vertices,
      ConstraintPoint{indexX0},
      ConstraintPoint{indexX1},
      ConstraintPoint{indexX2},
      ConstraintPoint{indexX3}
    ));
  }

  return solvingView;
}

U32 ClothPlane::GetVertexCount() const {
  return U32(m_vertices.GetSize());
}

U32 ClothPlane::GetIndexCount() const {
  return U32(m_triangles.GetSize() * 3);
}

U32 ClothPlane::TotalDataSize() const {
  return VertexDataSize() + IndexDataSize() + NormalDataSize();
}

void ClothPlane::AddEdgeTriangleNeighbor(const Edge& edge, const U32 triangleIdx) {
  const auto itr = m_edgeTriangleMap.find(edge);

  if (itr == m_edgeTriangleMap.end()) {
    m_edgeTriangleMap[edge] = std::vector<U32>();
    m_edgeTriangleMap[edge].reserve(2);
  }

  m_edgeTriangleMap[edge].push_back(triangleIdx);
}

} // namespace Physics
} // namespace Azura
