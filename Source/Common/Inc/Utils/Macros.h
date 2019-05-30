#pragma once

#if defined(WIN32) || defined(_WIN32)
#include <malloc.h>
#define ALIGNED_ALLOC(alignment, size) _aligned_malloc(size, alignment)

#define ALIGNED_FREE(ptr) _aligned_free(ptr)

#elif defined(UNIX) || defined(__linux__)
#include <malloc.h>
#define ALIGNED_ALLOC(alignment, size) memalign(alignment, size)

#define ALIGNED_FREE(ptr) free(ptr)

#elif defined(APPLE) || defined(__APPLE__)
#define ALIGNED_ALLOC(alignment, size) \
 [=] { \
  void* aPtr; \
  if (posix_memalign (&aPtr, alignment, size)) \
  { \
     aPtr = nullptr; \
  } \
  return aPtr;  \
  }()

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


// Credit Source:
// http://lucumr.pocoo.org/2013/8/18/beautiful-native-libraries/
#ifndef AZURA_API
#  ifdef _WIN32
#     if defined(YL_BUILD_SHARED) /* build dll */
#         define AZURA_API __declspec(dllexport)
#     elif !defined(YL_BUILD_STATIC) /* use dll */
#         define AZURA_API __declspec(dllimport)
#     else /* static library */
#         define AZURA_API
#     endif
#  else
#     if __GNUC__ >= 4
#         define AZURA_API __attribute__((visibility("default")))
#     else
#         define AZURA_API
#     endif
#  endif
#endif