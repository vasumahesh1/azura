#include "Generic/RawStorageFormat.h"
#include "Utils/Macros.h"

namespace Azura {

#define ENUM_SIZE_CASE(ENUM_TUPLE)          \
  FORWARD_MAPPING(GET_FULL_FORMAT_NAME_ONLY_FROM_TUPLE(ENUM_TUPLE), GET_FORMAT_WIDTH_ONLY_FROM_TUPLE(ENUM_TUPLE))

U32 GetFormatSize(RawStorageFormat format) {
  switch(format)
  {
    RAW_STORAGE_FORMAT_ITERATOR(ENUM_SIZE_CASE)

  default:
    return 0U;
  }
}
} // namespace Azura