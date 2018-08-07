#pragma once

#if defined(WIN32) || defined(_WIN32)
#include <malloc.h>
#define ALIGNED_ALLOC(alignment, size) \
  _aligned_malloc(size, alignment)

#define ALIGNED_FREE(ptr)  \
  _aligned_free(ptr)

#elif defined(UNIX) || defined(__linux__)
#include <malloc.h>
#define ALIGNED_ALLOC(alignment, size) \
  memalign(alignment, size)

#define ALIGNED_FREE(ptr) \
  free(ptr)

#elif defined(APPLE) || defined(__linux__)
#define ALIGNED_ALLOC(alignment, size) \
  memalign(alignment, size)

#define ALIGNED_FREE(ptr) \
  free(ptr)
#endif

#define UNUSED(x) (void)(x)