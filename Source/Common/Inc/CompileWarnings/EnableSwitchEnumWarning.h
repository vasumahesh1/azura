#include "Utils/Platform.h"

#ifdef AZURA_COMPILER_MSVC
#pragma warning(pop)
#elif AZURA_COMPILER_CLANG
#pragma clang diagnostic pop
#elif AZURA_COMPILER_GCC
#pragma GCC diagnostic pop
#endif