
.. _program_listing_file_Source_Azura_RenderSystem_Inc_Generic_GPUBuffer.h:

Program Listing for File GPUBuffer.h
====================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Inc_Generic_GPUBuffer.h>` (``Source\Azura\RenderSystem\Inc\Generic\GPUBuffer.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "Types.h"
   #include "Generic/GenericTypes.h"
   
   namespace Azura {
   
   struct BufferEntry {
     BufferEntry(U32 offset, U32 size, U32 maxSize, DescriptorType type);
   
     U32 m_offset;
     U32 m_size;
     U32 m_maxSize;
     DescriptorType m_type;
   };
   
   // A buffer that is on the GPU & holds mappings of the data it stores
   struct GPUBuffer {
   
     GPUBuffer(U32 size);
   
     // Size of the buffer
     U32 m_byteSize;
   };
   
   // A buffer that is mapped between CPU & GPU. Also, it holds mappings of the data it stores
   struct MappedGPUBuffer : public GPUBuffer {
   
     MappedGPUBuffer(U8* beginPtr, U32 maxEntries, U32 size, Memory::Allocator& allocator);
   
     bool SubAllocate(U32 size, U32 alignment);
   
     // Memory ptrs
     U8* p_dataCur;
     U8* p_dataBeg;
     U8* p_dataEnd;
   
     // Entry storage
     Containers::Vector<BufferEntry> m_entries;
   };
   
   } // namespace Azura
