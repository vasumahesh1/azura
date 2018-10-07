#include "Core/RawStorageFormat.h"
#include "Utils/Macros.h"

namespace Azura {

#define ENUM_SIZE_CASE(ENUM_TUPLE)                                                                                     \
  FORWARD_MAPPING(GET_FULL_FORMAT_NAME_FROM_TUPLE(ENUM_TUPLE), GET_FORMAT_WIDTH_FROM_TUPLE(ENUM_TUPLE))

#define ENUM_ASPECT_DEPTH_CHECK(ENUM_TUPLE)                                                                            \
  case GET_FULL_FORMAT_NAME_FROM_TUPLE(ENUM_TUPLE):                                                                    \
    return (GET_ASPECT_MASK_FROM_TUPLE(ENUM_TUPLE) & AspectMaskBits::DepthBit) == AspectMaskBits::DepthBit;

#define ENUM_ASPECT_STENCIL_CHECK(ENUM_TUPLE)                                                                            \
  case GET_FULL_FORMAT_NAME_FROM_TUPLE(ENUM_TUPLE):                                                                      \
    return (GET_ASPECT_MASK_FROM_TUPLE(ENUM_TUPLE) & AspectMaskBits::StencilBit) == AspectMaskBits::StencilBit;

U32 GetFormatSizeBits(RawStorageFormat format) {
  switch (format) {
    RAW_STORAGE_FORMAT_ITERATOR(ENUM_SIZE_CASE)

  default:
    return 0U;
  }
}

bool HasDepthComponent(RawStorageFormat format)
{
  switch (format) {
    RAW_STORAGE_FORMAT_ITERATOR(ENUM_ASPECT_DEPTH_CHECK)

  default:
    return false;
  }
}

bool HasStencilComponent(RawStorageFormat format)
{
  switch (format) {
    RAW_STORAGE_FORMAT_ITERATOR(ENUM_ASPECT_STENCIL_CHECK)

  default:
    return false;
  }
}

U32 GetFormatSize(RawStorageFormat format) {
  return GetFormatSizeBits(format) / 8;
}
}  // namespace Azura