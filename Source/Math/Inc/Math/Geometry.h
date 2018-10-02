#pragma once
#include "Types.h"
#include "Core/RawStorageFormat.h"

namespace Azura {
namespace Math {

class Geometry
{
public:
  Geometry() = default;
  virtual ~Geometry() = default;

  // Copy & Move
  Geometry(const Geometry& other) = delete;
  Geometry(Geometry&& other) noexcept = delete;
  Geometry& operator=(const Geometry& other) = delete;
  Geometry& operator=(Geometry&& other) noexcept = delete;

  // Overrides
  virtual U32 VertexDataSize() const = 0;
  virtual U32 IndexDataSize() const = 0;
  virtual U32 NormalDataSize() const = 0;
  virtual U32 TotalDataSize() const = 0;

  virtual U32 GetVertexCount() const = 0;
  virtual U32 GetIndexCount() const = 0;
  virtual const U8* VertexData() const = 0;
  virtual const U8* NormalData() const = 0;
  virtual const U8* IndexData() const = 0;
  virtual RawStorageFormat GetVertexFormat() const = 0;
  virtual RawStorageFormat GetNormalFormat() const = 0;
  virtual RawStorageFormat GetIndexFormat() const = 0;
};

} // namespace Math
} // namespace Azura