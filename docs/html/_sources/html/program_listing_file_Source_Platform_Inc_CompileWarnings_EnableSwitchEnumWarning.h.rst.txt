
.. _program_listing_file_Source_Platform_Inc_CompileWarnings_EnableSwitchEnumWarning.h:

Program Listing for File EnableSwitchEnumWarning.h
==================================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Platform_Inc_CompileWarnings_EnableSwitchEnumWarning.h>` (``Source\Platform\Inc\CompileWarnings\EnableSwitchEnumWarning.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Utils/Platform.h"
   
   #ifdef AZURA_COMPILER_MSVC
   #pragma warning(pop)
   #elif AZURA_COMPILER_CLANG
   #pragma clang diagnostic pop
   #elif AZURA_COMPILER_GCC
   #pragma GCC diagnostic pop
   #endif
