
.. _program_listing_file_Source_Platform_Inc_Utils_FileReader.h:

Program Listing for File FileReader.h
=====================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Platform_Inc_Utils_FileReader.h>` (``Source\Platform\Inc\Utils\FileReader.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "Types.h"
   
   #include "Containers/Vector.h"
   
   namespace Azura {
   namespace Memory {
   class Allocator;
   }
   } // namespace Azura
   
   namespace Azura {
   
   namespace FileReader {
   Containers::Vector<U8> GetFileContents(const String& filePath, Memory::Allocator& allocator);
   
   void GetFileContents(const String & filePath, Containers::Vector<U8>& buffer);
   
   SizeType GetFileSize(const String& filePath);
   } // namespace FileReader
   
   } // namespace Azura
