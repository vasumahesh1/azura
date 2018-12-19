
.. _program_listing_file_Source_Azura_RenderSystem_Inc_Generic_ComputePool.h:

Program Listing for File ComputePool.h
======================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Inc_Generic_ComputePool.h>` (``Source\Azura\RenderSystem\Inc\Generic\ComputePool.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "Containers/Vector.h"
   #include "GenericTypes.h"
   #include "Types.h"
   
   namespace Azura {
   struct TextureDesc;
   class Shader;
   
   struct ComputePoolCreateInfo {
     U32 m_byteSize{0};
   
     Containers::Vector<U32> m_computePasses;
     ThreadGroupDimensions m_launchDims;
   
     ~ComputePoolCreateInfo() = default;
   
     ComputePoolCreateInfo(const ComputePoolCreateInfo& other) = delete;
     ComputePoolCreateInfo(ComputePoolCreateInfo&& other) noexcept = delete;
     ComputePoolCreateInfo& operator=(const ComputePoolCreateInfo& other) = delete;
     ComputePoolCreateInfo& operator=(ComputePoolCreateInfo&& other) noexcept = delete;
   
     ComputePoolCreateInfo(Memory::Allocator& alloc);
   };
   
   class ComputePool {
   public:
     explicit ComputePool(const ComputePoolCreateInfo& createInfo,
                          DescriptorCount descriptorCount,
                          Memory::Allocator& allocator);
     virtual ~ComputePool() = default;
   
     ComputePool(const ComputePool& other) = delete;
     ComputePool(ComputePool&& other) noexcept = default;
     ComputePool& operator=(const ComputePool& other) = delete;
     ComputePool& operator=(ComputePool&& other) noexcept = default;
   
     // Drawable Scope Binds
     void BindUniformData(SlotID slot, const Containers::Vector<U8>& buffer);
     virtual void BindUniformData(SlotID slot, const U8* buffer, U32 size) = 0;
   
     // Pool Scope Binds
     virtual void AddShader(U32 shaderId) = 0;
     virtual void BindTextureData(SlotID slot, const TextureDesc& desc, const U8* buffer) = 0;
     virtual void BindSampler(SlotID slot, const SamplerDesc& desc) = 0;
   
     virtual void BeginUpdates() = 0;
     void UpdateUniformData(SlotID slot, const Containers::Vector<U8>& buffer);
     virtual void UpdateUniformData(SlotID slot, const U8* buffer, U32 size) = 0;
     virtual void UpdateTextureData(SlotID slot, const U8* buffer) = 0;
   
     virtual void Submit() = 0;
     virtual void SubmitUpdates() = 0;
   
     U32 GetSize() const;
     bool CanRenderInPass(U32 renderPassId) const;
   
   protected:
     U32 GetSingleUniformBufferInfo(const DescriptorSlot& slot);
     U32 GetSingleTextureBufferInfo(const DescriptorSlot& slot);
   
     Memory::Allocator& GetAllocator() const;
   
     DescriptorCount m_descriptorCount;
   
     Containers::Vector<U32> m_computePasses;
   
     Containers::Vector<UniformBufferInfo> m_uniformBufferInfos;
     Containers::Vector<TextureBufferInfo> m_textureBufferInfos;
     Containers::Vector<SamplerInfo> m_samplerInfos;
   
     Containers::Vector<BufferUpdate> m_bufferUpdates;
   
     ThreadGroupDimensions m_launchDims;
   
   private:
     U32 m_byteSize;
   
     std::reference_wrapper<Memory::Allocator> m_allocator;
   };
   
   } // namespace Azura
