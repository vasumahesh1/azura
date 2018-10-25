#pragma once

#if defined(WIN32) || defined(_WIN32)
#include <malloc.h>
#define ALIGNED_ALLOC(alignment, size) _aligned_malloc(size, alignment)

#define ALIGNED_FREE(ptr) _aligned_free(ptr)

#elif defined(UNIX) || defined(__linux__)
#include <malloc.h>
#define ALIGNED_ALLOC(alignment, size) memalign(alignment, size)

#define ALIGNED_FREE(ptr) free(ptr)

#elif defined(APPLE) || defined(__linux__)
#define ALIGNED_ALLOC(alignment, size) memalign(alignment, size)

#define ALIGNED_FREE(ptr) free(ptr)
#endif

#define UNUSED(x) (void)(x)

#define SCALE_SIZE(val, size) ((val + size - 1) & ~(size - 1))

#define FORWARD_MAPPING(TYPE_A, TYPE_B)                                                                                \
  case TYPE_A:                                                                                                         \
    return TYPE_B;

#define REVERSE_MAPPING(TYPE_A, TYPE_B)                                                                                \
  case TYPE_B:                                                                                                         \
    return TYPE_A;

#define VERIFY_TRUE(Logger, res, message)                                                            \
  if ((res) != true) {                                                                               \
    LOG_ERR(Logger, LOG_LEVEL, message);                                                             \
    throw std::runtime_error(message);                                                               \
  }

#define VERIFY_OPT(Logger, res, message)                                                             \
  if (!(res)) {                                                                                      \
    LOG_ERR(Logger, LOG_LEVEL, message);                                                             \
    throw std::runtime_error(message);                                                               \
  }

#define FAIL_IF(Logger, res, message)                                                                \
  if ((res)) {                                                                                       \
    LOG_ERR(Logger, LOG_LEVEL, message);                                                             \
    throw std::runtime_error(message);                                                               \
  }

#define ENUM_HAS(enumVar, value) \
  (enumVar & value) == value

#define CASE_STR(r)   \
    case r:           \
        return #r
