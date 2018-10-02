#include "Math/Icosphere.h"

#include <array>
#include <vector>
#include <map>

#include "Math/Transform.h"
#include "Core/RawStorageFormat.h"

namespace Azura {
namespace Math {
namespace {
// Source Ref: https://schneide.blog/2016/07/15/generating-an-icosphere-in-c/
const float X = 0.525731112119133606f;
const float Z = 0.850650808352039932f;
const float N = 0.0f;

const RawStorageFormat VERTEX_FORMAT = RawStorageFormat::R32G32B32A32_FLOAT;
const RawStorageFormat NORMAL_FORMAT = RawStorageFormat::R32G32B32_FLOAT;
const RawStorageFormat INDEX_FORMAT  = RawStorageFormat::R32_UINT;
using Lookup = std::map<std::pair<U32, U32>, U32>;

const std::array<Vector4f, 12> BASE_VERTICES =
{
  Vector4f{-X, N, Z, 1.0f}, Vector4f{X, N, Z, 1.0f}, Vector4f{-X, N, -Z, 1.0f}, Vector4f{X, N, -Z, 1.0f},
  Vector4f{N, Z, X, 1.0f}, Vector4f{N, Z, -X, 1.0f}, Vector4f{N, -Z, X, 1.0f}, Vector4f{N, -Z, -X, 1.0f},
  Vector4f{Z, X, N, 1.0f}, Vector4f{-Z, X, N, 1.0f}, Vector4f{Z, -X, N, 1.0f}, Vector4f{-Z, -X, N, 1.0f}
};

const std::array<Vector3u, 20> BASE_TRIANGLES =
{
  Vector3u{0, 4, 1}, Vector3u{0, 9, 4}, Vector3u{9, 5, 4}, Vector3u{4, 5, 8}, Vector3u{4, 8, 1},
  Vector3u{8, 10, 1}, Vector3u{8, 3, 10}, Vector3u{5, 3, 8}, Vector3u{5, 2, 3}, Vector3u{2, 7, 3},
  Vector3u{7, 10, 3}, Vector3u{7, 6, 10}, Vector3u{7, 11, 6}, Vector3u{11, 0, 6}, Vector3u{0, 1, 6},
  Vector3u{6, 1, 10}, Vector3u{9, 0, 11}, Vector3u{9, 11, 2}, Vector3u{9, 2, 5}, Vector3u{7, 2, 11}
};

U32 VertexForEdge(Lookup& lookup,
                  std::vector<Vector4f>& vertices,
                  U32 first,
                  U32 second) {

  Lookup::key_type key(first, second);

  if (key.first > key.second) {
    std::swap(key.first, key.second);
  }

  const auto inserted = lookup.insert({key, U32(vertices.size())});
  if (inserted.second) {
    auto& edge0         = vertices[first];
    auto& edge1         = vertices[second];
    const Vector3f data = Transform::Downgrade(edge0 + edge1);
    const auto point    = mathfu::NormalizedHelper(data);
    vertices.emplace_back(point, 1.0f);
  }

  return inserted.first->second;
}

std::vector<Vector3u> SubDivide(std::vector<Vector4f>& vertices,
                                std::vector<Vector3u> triangles) {
  Lookup lookup;
  std::vector<Vector3u> result;
  result.reserve(triangles.size() * 4);

  for (auto&& each : triangles) {
    Vector3u mid;
    for (int edge = 0; edge < 3; ++edge) {
      mid[edge]   = VertexForEdge(lookup, vertices, each[edge], each[(edge + 1) % 3]);
    }

    result.emplace_back(each[0], mid[0], mid[2]);
    result.emplace_back(each[1], mid[1], mid[0]);
    result.emplace_back(each[2], mid[2], mid[1]);
    result.emplace_back(mid[0], mid[1], mid[2]);
  }

  return result;
}
} // namespace

IcoSphere::IcoSphere(U32 subDivisions)
  : m_vertices(BASE_VERTICES.begin(), BASE_VERTICES.end()),
    m_triangles(BASE_TRIANGLES.begin(), BASE_TRIANGLES.end()) {

  m_vertices.reserve(10 * U32(std::pow(4, subDivisions)));
  m_normals.reserve(10 * U32(std::pow(4, subDivisions)));
  m_triangles.reserve(10 * U32(std::pow(4, subDivisions)));

  for (U32 idx  = 0; idx < subDivisions; ++idx) {
    m_triangles = SubDivide(m_vertices, m_triangles);
  }

  for (const auto& vertex: m_vertices)
  {
    m_normals.push_back(Transform::Downgrade(vertex).Normalized());
  }
}

U32 IcoSphere::VertexDataSize() const {
  return U32(m_vertices.size() * GetFormatSize(VERTEX_FORMAT));
}

U32 IcoSphere::IndexDataSize() const {
  return U32(m_triangles.size() * GetFormatSize(INDEX_FORMAT) * 3);
}

U32 IcoSphere::NormalDataSize() const {
  return U32(m_normals.size() * GetFormatSize(NORMAL_FORMAT));
}

const U8* IcoSphere::VertexData() const {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<const U8*>(m_vertices.data());
}

const U8* IcoSphere::IndexData() const {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<const U8*>(m_triangles.data());
}

const U8* IcoSphere::NormalData() const {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<const U8*>(m_normals.data());
}

RawStorageFormat IcoSphere::GetVertexFormat() const {
  return VERTEX_FORMAT;
}

RawStorageFormat IcoSphere::GetIndexFormat() const {
  return INDEX_FORMAT;
}

RawStorageFormat IcoSphere::GetNormalFormat() const {
  return NORMAL_FORMAT;
}

U32 IcoSphere::GetVertexCount() const {
  return U32(m_vertices.size());
}

U32 IcoSphere::GetIndexCount() const {
  return U32(m_triangles.size() * 3);
}

U32 IcoSphere::TotalDataSize() const {
  return VertexDataSize() + IndexDataSize() + NormalDataSize();
}

} // namespace Math
} // namespace Azura
