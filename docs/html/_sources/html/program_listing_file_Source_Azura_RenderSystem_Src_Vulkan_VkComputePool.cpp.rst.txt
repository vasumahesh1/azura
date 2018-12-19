
.. _program_listing_file_Source_Azura_RenderSystem_Src_Vulkan_VkComputePool.cpp:

Program Listing for File VkComputePool.cpp
==========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Src_Vulkan_VkComputePool.cpp>` (``Source\Azura\RenderSystem\Src\Vulkan\VkComputePool.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Vulkan/VkComputePool.h"
   
   namespace Azura {
   namespace Vulkan {
   
   VkComputePool::VkComputePool(const ComputePoolCreateInfo& createInfo,
                                const DescriptorCount& descriptorCount,
                                Memory::Allocator& allocator)
     : ComputePool(createInfo, descriptorCount, allocator) {
   }
   
   void VkComputePool::BindUniformData(SlotID slot, const U8* buffer, U32 size) {
     UNUSED(slot);
     UNUSED(buffer);
     UNUSED(size);
   }
   
   void VkComputePool::AddShader(U32 shaderId) {
     UNUSED(shaderId);
   }
   
   void VkComputePool::BindTextureData(SlotID slot, const TextureDesc& desc, const U8* buffer) {
     UNUSED(slot);
     UNUSED(buffer);
     UNUSED(desc);
   }
   
   void VkComputePool::BindSampler(SlotID slot, const SamplerDesc& desc) {
     UNUSED(slot);
     UNUSED(desc);
   }
   
   void VkComputePool::BeginUpdates() {
   }
   
   void VkComputePool::UpdateUniformData(SlotID slot, const U8* buffer, U32 size) {
     UNUSED(slot);
     UNUSED(buffer);
     UNUSED(size);
   }
   
   void VkComputePool::UpdateTextureData(SlotID slot, const U8* buffer) {
     UNUSED(slot);
     UNUSED(buffer);
   }
   
   void VkComputePool::Submit() {
   }
   
   void VkComputePool::SubmitUpdates() {
   }
   } // namespace Vulkan
   } // namespace Azura
