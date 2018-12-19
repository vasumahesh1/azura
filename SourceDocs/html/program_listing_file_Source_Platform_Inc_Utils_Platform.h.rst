
.. _program_listing_file_Source_Platform_Inc_Utils_Platform.h:

Program Listing for File Platform.h
===================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Platform_Inc_Utils_Platform.h>` (``Source\Platform\Inc\Utils\Platform.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #ifdef _WIN32
      // Win32 & Win64
      #define AZURA_PLATFORM_WINDOWS
      #ifdef _WIN64
         // Win64 Only
      #else
         // Win32 Only
      #endif
   
   #elif __APPLE__
       #include "TargetConditionals.h"
       #if TARGET_OS_IPHONE
           // iOS device
       #elif TARGET_OS_MAC
           // Mac OS
           #define AZURA_PLATFORM_MACOS
       #else
       #   error "Unknown Apple platform"
       #endif
   #elif __linux__
   // Linux
   #define AZURA_PLATFORM_LINUX
   #elif __unix__
   // Unix
   #define AZURA_PLATFORM_UNIX
   #elif defined(_POSIX_VERSION)
   // POSIX
   #define AZURA_PLATFORM_POSIX
   #else
   #   error "Unknown compiler"
   #endif
   
   
   #if defined(__clang__)
   #define AZURA_COMPILER_CLANG
   #elif defined(__GNUC__) || defined(__GNUG__)
   #define AZURA_COMPILER_GCC
   #elif defined(_MSC_VER)
   #define AZURA_COMPILER_MSVC
   #endif
