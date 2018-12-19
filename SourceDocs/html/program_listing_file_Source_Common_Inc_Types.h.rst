
.. _program_listing_file_Source_Common_Inc_Types.h:

Program Listing for File Types.h
================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Common_Inc_Types.h>` (``Source\Common\Inc\Types.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <cstddef>
   #include <cstdint>
   #include <string>
   
   namespace Azura {
   using UINT        = unsigned int;
   using U8          = std::uint8_t;
   using U16         = std::uint16_t;
   using U32         = std::uint32_t;
   using U64         = std::uint64_t;
   using UPTR        = std::uintptr_t;
   using SizeType    = std::size_t;
   using String      = std::string;
   using AddressPtr  = UPTR;
   using AddressDiff = ptrdiff_t;
   }  // namespace Azura
