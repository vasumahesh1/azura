
.. _program_listing_file_Source_Platform_Inc_CompileWarnings_DisableSwitchEnumWarning.h:

Program Listing for File DisableSwitchEnumWarning.h
===================================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Platform_Inc_CompileWarnings_DisableSwitchEnumWarning.h>` (``Source\Platform\Inc\CompileWarnings\DisableSwitchEnumWarning.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

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
