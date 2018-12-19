
.. _program_listing_file_Source_Azura_RenderSystem_Inc_Vulkan_VkComputePool.h:

Program Listing for File VkComputePool.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Inc_Vulkan_VkComputePool.h>` (``Source\Azura\RenderSystem\Inc\Vulkan\VkComputePool.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <vulkan/vulkan_core.h>
   
   #include "Generic/Drawable.h"
   #include "Generic/ComputePool.h"
   
   
   namespace Azura {
   namespace Vulkan {
   
   class VkComputePool : public ComputePool {
   
   public:
     VkComputePool(const ComputePoolCreateInfo& createInfo,
                   const DescriptorCount& descriptorCount,
                   Memory::Allocator& allocator);
   
     void BindUniformData(SlotID slot, const U8* buffer, U32 size) override;
     void AddShader(U32 shaderId) override;
     void BindTextureData(SlotID slot, const TextureDesc& desc, const U8* buffer) override;
     void BindSampler(SlotID slot, const SamplerDesc& desc) override;
     void BeginUpdates() override;
     void UpdateUniformData(SlotID slot, const U8* buffer, U32 size) override;
     void UpdateTextureData(SlotID slot, const U8* buffer) override;
     void Submit() override;
     void SubmitUpdates() override;
   };
   
   } // namespace Vulkan
   } // namespace Azura
