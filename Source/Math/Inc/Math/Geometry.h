#pragma once
#include "Types.h"
#include "Core/RawStorageFormat.h"

namespace Azura {
namespace Math {

constexpr float PI = 3.14159265358979323846f;
constexpr float TWO_PI = 6.28318530717958647692f;
constexpr float INV_PI = 0.31830988618379067154f;
constexpr float INV2_PI = 0.15915494309189533577f;
constexpr float INV4_PI = 0.07957747154594766788f;
constexpr float PI_OVER2 = 1.57079632679489661923f;
constexpr float PI_OVER4 = 0.78539816339744830961f;
constexpr float SQRT_2 = 1.41421356237309504880f;
constexpr float ONE_MINUS_EPSILON = 0.99999994f;
constexpr float ONE_DEGREE_RADIAN = 0.0174533f;

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
  virtual U32 UVDataSize() const = 0;
  virtual U32 TotalDataSize() const = 0;

  virtual U32 GetVertexCount() const = 0;
  virtual U32 GetIndexCount() const = 0;
  virtual const U8* VertexData() const = 0;
  virtual const U8* NormalData() const = 0;
  virtual const U8* IndexData() const = 0;
  virtual const U8* UVData() const = 0;
  virtual RawStorageFormat GetVertexFormat() const = 0;
  virtual RawStorageFormat GetNormalFormat() const = 0;
  virtual RawStorageFormat GetIndexFormat() const = 0;
  virtual RawStorageFormat GetUVFormat() const = 0;
};

float ToRadians(float degrees);
float ToAngle(float radians);

} // namespace Math
} // namespace Azura