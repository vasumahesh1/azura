
.. _program_listing_file_Source_Common_Inc_Memory_MemoryFactory.h:

Program Listing for File MemoryFactory.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Common_Inc_Memory_MemoryFactory.h>` (``Source\Common\Inc\Memory\MemoryFactory.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "Memory/StackMemoryBuffer.h"
   #include "Memory/HeapMemoryBuffer.h"
   #include "Types.h"
   
   namespace Azura {
   #define STACK_ALLOCATOR(NAME, TYPE, SIZE)                                                                              \
     Memory::StackMemoryBuffer<SIZE> buffer##NAME;                                                                        \
     TYPE allocator##NAME = TYPE(buffer##NAME, SIZE);
   
   #define HEAP_ALLOCATOR(NAME, TYPE, SIZE)                                                                              \
     Memory::HeapMemoryBuffer buffer##NAME = Memory::HeapMemoryBuffer(SIZE);                                                     \
     TYPE allocator##NAME = TYPE(buffer##NAME, SIZE);
   }  // namespace Azura
