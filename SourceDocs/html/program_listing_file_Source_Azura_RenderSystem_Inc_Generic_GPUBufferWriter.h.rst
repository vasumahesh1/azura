
.. _program_listing_file_Source_Azura_RenderSystem_Inc_Generic_GPUBufferWriter.h:

Program Listing for File GPUBufferWriter.h
==========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Inc_Generic_GPUBufferWriter.h>` (``Source\Azura\RenderSystem\Inc\Generic\GPUBufferWriter.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "Generic/GPUBuffer.h"
   
   namespace Azura {
   class Log;
   
   namespace GPUBufferWriter {
   U32 AppendRaw(MappedGPUBuffer& buffer,
                 DescriptorType type,
                 void* data,
                 U32 byteSize,
                 U32 alignment,
                 const Log& log);
   
   U32 AppendRaw(MappedGPUBuffer& buffer,
                 DescriptorType type,
                 void* data,
                 U32 byteSize,
                 U32 maxSize,
                 U32 alignment,
                 const Log& log);
   
   U32 AppendTexture(MappedGPUBuffer& buffer,
                     void* data,
                     U32 byteSize,
                     U32 alignment,
                     U32 currentRowPitch,
                     U32 requiredRowPitch,
                     const Log& log);
   };
   
   } // namespace Azura
