#include "Physics/Geometry/ClothMesh.h"

#include "Physics/Geometry/GenericTypes.h"

namespace Azura {
namespace Physics {

using namespace PBD; // NOLINT

namespace {
const RawStorageFormat VERTEX_FORMAT = RawStorageFormat::R32G32B32_FLOAT;
const RawStorageFormat UV_FORMAT     = RawStorageFormat::R32G32_FLOAT;
const RawStorageFormat NORMAL_FORMAT = RawStorageFormat::R32G32B32_FLOAT;
const RawStorageFormat INDEX_FORMAT  = RawStorageFormat::R32_UINT;

float ComputeBendingC(const std::vector<Vector4f>& vertices, U32 dest1, U32 dest2, U32 source) {
  Vector3f v1 = vertices[dest1].xyz() - vertices[source].xyz();
  Vector3f v2 = vertices[dest2].xyz() - vertices[source].xyz();

  return Vector3f::DotProduct(v1, v2) / Vector3f::CrossProduct(v1, v2).Length();
}

} // namespace

ClothMesh::ClothMesh(const String& assetName, AssetLocation location, Memory::Allocator& allocator, const Log& log)
  : m_vertices(allocator),
    m_vertexInvMass(allocator),
    m_vertexAlias(allocator),
    m_normals(allocator),
    m_triangles(allocator),
    m_uv(allocator) {

  p_interface = std::make_unique<GLTFMeshInterface>(GLTFLoader::LoadFromJSON(assetName, location, log));

  U32 vertexDataSize;
  U32 numVertices;

  U32 normalDataSize;
  U32 numNormals;

  U32 uvDataSize;
  U32 numUV;

  U32 indexDataSize;
  U32 numIndices;

  const auto vertexData = p_interface->GetPositionBuffer(0, 0, vertexDataSize, numVertices);
  const auto normalData = p_interface->GetNormalBuffer(0, 0, normalDataSize, numNormals);
  const auto uvData     = p_interface->GetUVBuffer(0, 0, uvDataSize, numUV);
  const auto indexData  = p_interface->GetIndexBuffer(0, 0, indexDataSize, numIndices);

  const U32 numTriangles = numIndices / 3;

  m_vertices.Reserve(numVertices);
  m_normals.Reserve(numVertices);
  m_uv.Reserve(numVertices);
  m_triangles.Reserve(numTriangles);
  m_vertexAlias.Reserve(numVertices);

  for (U32 idx = 0; idx < numVertices; ++idx) {

    const U32 id0 = 3 * idx;
    const U32 id1 = 3 * idx + 1;
    const U32 id2 = 3 * idx + 2;

    const float vx = vertexData[id0];
    const float vy = vertexData[id1];
    const float vz = vertexData[id2];

    const float nx = normalData[id0];
    const float ny = normalData[id1];
    const float nz = normalData[id2];

    const float uvx = uvData[2 * idx];
    const float uvy = uvData[2 * idx + 1];

    const Vector3f key = Vector3f(vx, vy, vz);
    if (m_vertexAliasMap.find(key) == m_vertexAliasMap.end())
    {
      m_vertexAliasMap[key] = idx;
      m_vertexAlias.PushBack(-1);
    }
    else
    {
      m_vertexAlias.PushBack(m_vertexAliasMap[key]);
    }

    m_vertices.EmplaceBack(vx, vy, vz);
    m_normals.EmplaceBack(nx, ny, nz);
    m_uv.EmplaceBack(uvx, uvy);
  }

  m_vertexInvMass.Reserve(m_vertices.GetSize());

  for (U32 idx    = 0; idx < numTriangles; ++idx) {
    const U32 id0 = indexData[3 * idx];
    const U32 id1 = indexData[3 * idx + 1];
    const U32 id2 = indexData[3 * idx + 2];

    const auto triangleIdx = U32(m_triangles.GetSize());

    m_triangles.EmplaceBack(id0, id1, id2);

    AddEdgeTriangleNeighbor(Edge{id0, id1}, triangleIdx);
    AddEdgeTriangleNeighbor(Edge{id1, id2}, triangleIdx);
    AddEdgeTriangleNeighbor(Edge{id2, id0}, triangleIdx);
  }
}

U32 ClothMesh::VertexDataSize() const {
  return U32(m_vertices.GetSize() * GetFormatSize(VERTEX_FORMAT));
}

U32 ClothMesh::IndexDataSize() const {
  return U32(m_triangles.GetSize() * GetFormatSize(INDEX_FORMAT) * 3);
}

U32 ClothMesh::NormalDataSize() const {
  return U32(m_normals.GetSize() * GetFormatSize(NORMAL_FORMAT));
}

U32 ClothMesh::UVDataSize() const {
  return U32(m_vertices.GetSize() * GetFormatSize(UV_FORMAT));
}

const U8* ClothMesh::VertexData() const {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<const U8*>(m_vertices.Data());
}

const U8* ClothMesh::IndexData() const {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<const U8*>(m_triangles.Data());
}

const U8* ClothMesh::NormalData() const {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<const U8*>(m_normals.Data());
}

const U8* ClothMesh::UVData() const {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<const U8*>(m_uv.Data());
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

RawStorageFormat ClothMesh::GetUVFormat() const {
  return UV_FORMAT;
}

void ClothMesh::SetAnchorOnIndex(U32 idx) {
  m_anchorIdx.push_back(idx);
}

const Containers::Vector<float>& ClothMesh::GetVertexInverseMass() const {
  return m_vertexInvMass;
}

const Containers::Vector<int>& ClothMesh::GetVertexAliases() const {
  return m_vertexAlias;
}

ClothSolvingView ClothMesh::GetPBDSolvingView(Memory::Allocator& allocator) {
  for (SizeType idx = 0; idx < m_vertices.GetSize(); ++idx) {
    if (IsVertexAnchorPoint(idx)) {
      m_vertexInvMass.PushBack(0.0f);
      continue;
    }

    m_vertexInvMass.PushBack(1.0f);
  }

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
                                                  U32(m_edgeTriangleMap.size()),
                                                  numBendingConstraints,
                                                  allocator
                                                 );

  U32 vertIdx = 0;
  for (const auto& vertex : m_vertices) {

    if (m_vertexAlias[vertIdx] != -1)
    {
      ++vertIdx;
      continue;
    }

    for (const auto& anchorIdx : m_anchorIdx) {
      const float distance = (m_vertices[anchorIdx] - vertex).Length();
      solvingView.AddConstraint(LongRangeConstraint{
        ConstraintPoint{vertIdx},
        ConstraintPoint{anchorIdx},
        distance
        });
    }

    ++vertIdx;
  }

  for (const auto& pair : m_edgeTriangleMap) {
    const Edge& edge = pair.first;

    // Add Distance Constraint
    solvingView.AddConstraint(DistanceConstraint{
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
      const U32 aliasIdx1 = m_vertexAlias[tri1[idx]] == -1 ? tri1[idx] : m_vertexAlias[tri1[idx]];

      if (aliasIdx1 == edge.m_indexA) {
        continue;
      }

      if (aliasIdx1 == edge.m_indexB) {
        continue;
      }

      indexX2 = aliasIdx1;
    }

    for (U32 idx = 0; idx < 3; ++idx) {
      const U32 aliasIdx2 = m_vertexAlias[tri2[idx]] == -1 ? tri2[idx] : m_vertexAlias[tri2[idx]];

      if (aliasIdx2 == edge.m_indexA) {
        continue;
      }

      if (aliasIdx2 == edge.m_indexB) {
        continue;
      }

      indexX3 = aliasIdx2;
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

U32 ClothMesh::GetVertexCount() const {
  return U32(m_vertices.GetSize());
}

U32 ClothMesh::GetIndexCount() const {
  return U32(m_triangles.GetSize() * 3);
}

U32 ClothMesh::TotalDataSize() const {
  return VertexDataSize() + IndexDataSize() + NormalDataSize() + UVDataSize();
}

void ClothMesh::AddEdgeTriangleNeighbor(const Edge& edge, const U32 triangleIdx) {
  Edge copyEdge = {};
  copyEdge.m_indexA = m_vertexAlias[edge.m_indexA] == -1 ? edge.m_indexA : m_vertexAlias[edge.m_indexA];
  copyEdge.m_indexB = m_vertexAlias[edge.m_indexB] == -1 ? edge.m_indexB : m_vertexAlias[edge.m_indexB];

  const auto itr = m_edgeTriangleMap.find(copyEdge);

  if (itr == m_edgeTriangleMap.end()) {
    m_edgeTriangleMap[copyEdge] = std::vector<U32>();
    m_edgeTriangleMap[copyEdge].reserve(2);
  }

  m_edgeTriangleMap[copyEdge].push_back(triangleIdx);
}

bool ClothMesh::IsVertexAnchorPoint(SizeType idx) const {
  return std::find(m_anchorIdx.begin(), m_anchorIdx.end(), idx) != m_anchorIdx.end();
}

} // namespace Physics
} // namespace Azura
