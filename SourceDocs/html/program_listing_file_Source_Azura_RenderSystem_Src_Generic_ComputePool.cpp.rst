
.. _program_listing_file_Source_Azura_RenderSystem_Src_Generic_ComputePool.cpp:

Program Listing for File ComputePool.cpp
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Src_Generic_ComputePool.cpp>` (``Source\Azura\RenderSystem\Src\Generic\ComputePool.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Generic/ComputePool.h"
   #include <algorithm>
   #include "Log/Log.h"
   #include "Generic/TextureManager.h"
   #include "Containers/Vector.h"
   
   namespace Azura {
   
   ComputePoolCreateInfo::ComputePoolCreateInfo(Memory::Allocator& alloc)
     : m_computePasses(alloc) {
   }
   
   ComputePool::ComputePool(const ComputePoolCreateInfo& createInfo,
                            DescriptorCount descriptorCount,
                            Memory::Allocator& allocator)
     : m_descriptorCount(descriptorCount),
       m_computePasses(createInfo.m_computePasses, allocator),
       m_uniformBufferInfos(descriptorCount.m_numUniformSlots, allocator),
       m_textureBufferInfos(allocator),
       m_samplerInfos(allocator),
       m_bufferUpdates(allocator),
       m_launchDims(createInfo.m_launchDims),
       m_byteSize(createInfo.m_byteSize),
       m_allocator(allocator) {
   
     m_bufferUpdates.Reserve(m_descriptorCount.m_numSampledImageSlots + m_descriptorCount.m_numUniformSlots);
   
     m_textureBufferInfos.Reserve(m_descriptorCount.m_numSampledImageSlots);
     m_samplerInfos.Reserve(m_descriptorCount.m_numSamplerSlots);
   }
   
   void ComputePool::BindUniformData(SlotID slot, const Containers::Vector<U8>& buffer) {
     BindUniformData(slot, buffer.Data(), buffer.GetSize());
   }
   
   void ComputePool::UpdateUniformData(SlotID slot, const Containers::Vector<U8>& buffer) {
     UpdateUniformData(slot, buffer.Data(), buffer.GetSize());
   }
   
   U32 ComputePool::GetSingleUniformBufferInfo(const DescriptorSlot& slot) {
     U32 idx = 0;
     for (const auto& ubInfo : m_uniformBufferInfos) {
       if (ubInfo.m_binding == slot.m_bindIdx && ubInfo.m_set == slot.m_setIdx) {
         return idx;
       }
   
       ++idx;
     }
   
     throw std::runtime_error("Tried to Update Slot that was never bound");
   }
   
   U32 ComputePool::GetSingleTextureBufferInfo(const DescriptorSlot& slot) {
     U32 idx = 0;
     for (const auto& textureBufferInfo : m_textureBufferInfos) {
       if (textureBufferInfo.m_binding == slot.m_bindIdx && textureBufferInfo.m_set == slot.m_setIdx) {
         return idx;
       }
   
       ++idx;
     }
   
     throw std::runtime_error("Tried to Update Slot that was never bound");
   
   }
   
   U32 ComputePool::GetSize() const {
     return m_byteSize;
   }
   
   Memory::Allocator& ComputePool::GetAllocator() const {
     return m_allocator;
   }
   
   bool ComputePool::CanRenderInPass(U32 renderPassId) const {
     for (const auto& pass : m_computePasses) {
       if (pass == renderPassId) {
         return true;
       }
     }
   
     return false;
   }
   } // namespace Azura
