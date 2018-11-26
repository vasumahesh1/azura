#include "Math/Plane.h"

namespace Azura {
namespace Math {

namespace {
const RawStorageFormat VERTEX_FORMAT = RawStorageFormat::R32G32B32A32_FLOAT;
const RawStorageFormat NORMAL_FORMAT = RawStorageFormat::R32G32B32_FLOAT;
const RawStorageFormat UV_FORMAT = RawStorageFormat::R32G32_FLOAT;
const RawStorageFormat INDEX_FORMAT  = RawStorageFormat::R32_UINT;
} // namespace

Plane::Plane(const Vector2f& boundMin, const Vector2f& boundMax)
  : Plane(boundMin, boundMax, Vector2u(1, 1), Vector2u(1, 1)) {
}

Plane::Plane(const Vector2f& boundMin, const Vector2f& boundMax, const Vector2u& subDivisions, const Vector2u& uvScale) {
  const float stepX = float(boundMax[0] - boundMin[0]) / subDivisions[0];
  const float stepY = float(boundMax[1] - boundMin[1]) / subDivisions[1];

  m_vertices.reserve((subDivisions[0] + 1) * (subDivisions[1] + 1));
  m_normals.reserve(m_vertices.size());
  m_uv.reserve(m_vertices.size());

  U32 xCount = 0;
  for (float xCoord   = boundMin[0]; xCoord <= boundMax[0] + EPSILON;) {
    U32 yCount = 0;
    for (float yCoord = boundMin[1]; yCoord <= boundMax[1] + EPSILON;) {
      m_vertices.emplace_back(xCoord, 0.0f, yCoord, 1.0f);
      m_normals.emplace_back(0.0f, 1.0f, 0.0f);
      yCoord += stepY;

      m_uv.emplace_back(uvScale[0] * float(xCount) / subDivisions[0], uvScale[1] * float(yCount) / subDivisions[1]);
      ++yCount;
    }

    ++xCount;
    xCoord += stepX;
  }

  for (U32 idx     = 0; idx < subDivisions[0]; ++idx) {
    for (U32 idy   = 0; idy < subDivisions[1]; ++idy) {
      const U32 i1 = ((subDivisions[1] + 1) * idx) + idy;
      const U32 i2 = ((subDivisions[1] + 1) * (idx + 1)) + idy;
      const U32 i21 = i2 + 1;
      const U32 i11 = i1 + 1;

      m_triangles.emplace_back(Vector3u(i1, i2, i21));
      m_triangles.emplace_back(Vector3u(i1, i21, i11));
    }
  }
}

U32 Plane::VertexDataSize() const {
  return U32(m_vertices.size() * GetFormatSize(VERTEX_FORMAT));
}

U32 Plane::IndexDataSize() const {
  return U32(m_triangles.size() * GetFormatSize(INDEX_FORMAT) * 3);
}

U32 Plane::UVDataSize() const {
  return U32(m_uv.size() * GetFormatSize(UV_FORMAT));
}

U32 Plane::NormalDataSize() const {
  return U32(m_normals.size() * GetFormatSize(NORMAL_FORMAT));
}

const U8* Plane::VertexData() const {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<const U8*>(m_vertices.data());
}

const U8* Plane::IndexData() const {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<const U8*>(m_triangles.data());
}

const U8* Plane::UVData() const {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<const U8*>(m_uv.data());
}

const U8* Plane::NormalData() const {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<const U8*>(m_normals.data());
}

RawStorageFormat Plane::GetVertexFormat() const {
  return VERTEX_FORMAT;
}

RawStorageFormat Plane::GetIndexFormat() const {
  return INDEX_FORMAT;
}

RawStorageFormat Plane::GetNormalFormat() const {
  return NORMAL_FORMAT;
}

RawStorageFormat Plane::GetUVFormat() const {
  return UV_FORMAT;
}

U32 Plane::GetVertexCount() const {
  return U32(m_vertices.size());
}

U32 Plane::GetIndexCount() const {
  return U32(m_triangles.size() * 3);
}

U32 Plane::TotalDataSize() const {
  return VertexDataSize() + IndexDataSize() + NormalDataSize() + UVDataSize();
}

const std::vector<Vector4f>& Plane::GetVertices() const {
  return m_vertices;
}

std::vector<Vector4f>& Plane::GetVertices() {
  return m_vertices;
}

const std::vector<Vector3f>& Plane::GetNormals() const {
  return m_normals;
}

std::vector<Vector3f>& Plane::GetNormals() {
  return m_normals;
}
} // namespace Math
} // namespace Azura
