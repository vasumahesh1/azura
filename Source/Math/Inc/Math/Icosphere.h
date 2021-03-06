#pragma once
#include "Types.h"
#include "Geometry.h"
#include "Math/Core.h"
#include <vector>

namespace Azura {
namespace Math {

class IcoSphere final : public Geometry {
public:
  IcoSphere(U32 subDivisions);

  U32 VertexDataSize() const override;
  U32 IndexDataSize() const override;
  const U8* VertexData() const override;
  const U8* IndexData() const override;
  RawStorageFormat GetVertexFormat() const override;
  RawStorageFormat GetIndexFormat() const override;
  U32 GetVertexCount() const override;
  U32 GetIndexCount() const override;
  U32 TotalDataSize() const override;

  U32 NormalDataSize() const override;
  const U8* NormalData() const override;
  RawStorageFormat GetNormalFormat() const override;

  U32 UVDataSize() const override;
  const U8* UVData() const override;
  RawStorageFormat GetUVFormat() const override;
private:
  std::vector<Vector4f> m_vertices;
  std::vector<Vector3f> m_normals{};
  std::vector<Vector3u> m_triangles;
};
} // namespace Math
} // namespace Azura
