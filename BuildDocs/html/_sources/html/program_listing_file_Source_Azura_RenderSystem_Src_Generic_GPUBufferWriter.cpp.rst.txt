
.. _program_listing_file_Source_Azura_RenderSystem_Src_Generic_GPUBufferWriter.cpp:

Program Listing for File GPUBufferWriter.cpp
============================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Src_Generic_GPUBufferWriter.cpp>` (``Source\Azura\RenderSystem\Src\Generic\GPUBufferWriter.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Generic/GPUBufferWriter.h"
   #include "Log/Log.h"
   #include "Generic/Debug.h"
   
   namespace Azura {
   
   
   U32 GPUBufferWriter::AppendRaw(MappedGPUBuffer& buffer,
                                  DescriptorType type,
                                  void* data,
                                  U32 byteSize,
                                  U32 alignment,
                                  const Log& log) {
     return AppendRaw(buffer, type, data, byteSize, byteSize, alignment, log);
   }
   
   U32 GPUBufferWriter::AppendRaw(MappedGPUBuffer& buffer,
                                  DescriptorType type,
                                  void* data,
                                  U32 byteSize,
                                  U32 maxSize,
                                  U32 alignment,
                                  const Log& log) {
     assert(alignment != 0);
     assert(byteSize != 0);
     assert(maxSize != 0);
     assert(data != nullptr);
     assert(byteSize <= maxSize);
   
     LOG_DEBUG(log, LOG_LEVEL, "Writing Descriptor (%s) Data to Buffer as Raw: %d - %d bytes with alignment %d",
       DescriptorTypeToString(type), byteSize, maxSize, alignment);
   
     VERIFY_TRUE(log, buffer.SubAllocate(maxSize, alignment), "Failed to SubAllocate in GPU Mapped Buffer");
   
     const auto byteOffset = U32(buffer.p_dataCur - buffer.p_dataBeg);
   
     std::memcpy(buffer.p_dataCur, data, byteSize);
     buffer.p_dataCur += maxSize; // NOLINT
   
     const U32 returnId = buffer.m_entries.GetSize();
     buffer.m_entries.EmplaceBack(byteOffset, byteSize, maxSize, type);
   
     LOG_DEBUG(log, LOG_LEVEL, "Writing Descriptor (%s) completed", DescriptorTypeToString(type));
     return returnId;
   }
   
   U32 GPUBufferWriter::AppendTexture(MappedGPUBuffer& buffer,
                                      void* data,
                                      U32 byteSize,
                                      U32 alignment,
                                      U32 currentRowPitch,
                                      U32 requiredRowPitch,
                                      const Log& log) {
     assert(data != nullptr);
     assert(currentRowPitch != 0);
     assert(requiredRowPitch != 0);
     assert(alignment != 0);
     assert(byteSize != 0);
   
     const U32 numRows        = byteSize / currentRowPitch;
     const U32 scaledRowPitch = SCALE_SIZE(currentRowPitch, requiredRowPitch);
     const U32 requiredSize   = scaledRowPitch * numRows;
   
     LOG_DEBUG(log, LOG_LEVEL, "Writing Texture Data to Buffer as Raw: %d bytes with alignment %d", byteSize, alignment);
     LOG_DEBUG(log, LOG_LEVEL, "Scaled Texture Row Pitch from %d to %d", currentRowPitch, scaledRowPitch);
   
     VERIFY_TRUE(log, buffer.SubAllocate(requiredSize, alignment), "Failed to SubAllocate in GPU Mapped Buffer");
   
     const auto byteOffset = U32(buffer.p_dataCur - buffer.p_dataBeg);
   
     const U8* textureData = reinterpret_cast<const U8*>(data); // NOLINT
   
     for (U32 idx = 0; idx < numRows; ++idx) {
       std::memcpy(buffer.p_dataCur, textureData, currentRowPitch);
   
       buffer.p_dataCur += scaledRowPitch; // NOLINT
       textureData += currentRowPitch;     // NOLINT
     }
   
     const U32 returnId = buffer.m_entries.GetSize();
     buffer.m_entries.EmplaceBack(byteOffset, byteSize, byteSize, DescriptorType::SampledImage);
   
     LOG_DEBUG(log, LOG_LEVEL, "Writing Texture completed");
     return returnId;
   }
   } // namespace Azura
