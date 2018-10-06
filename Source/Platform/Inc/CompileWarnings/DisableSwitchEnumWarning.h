#include "Utils/Platform.h"

#ifdef AZURA_COMPILER_MSVC
#pragma warning(push)  
#pragma warning(disable : 4265) 
#elif AZURA_COMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch-enum"
#elif AZURA_COMPILER_GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
#endif