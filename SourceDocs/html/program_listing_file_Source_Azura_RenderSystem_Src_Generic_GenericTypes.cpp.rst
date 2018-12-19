
.. _program_listing_file_Source_Azura_RenderSystem_Src_Generic_GenericTypes.cpp:

Program Listing for File GenericTypes.cpp
=========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Src_Generic_GenericTypes.cpp>` (``Source\Azura\RenderSystem\Src\Generic\GenericTypes.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Generic/GenericTypes.h"
   
   namespace Azura {
   
   ShaderRequirements::ShaderRequirements(U32 numShaders, Memory::Allocator& allocator)
     : m_shaders(numShaders, allocator) {
   
   }
   
   U32 ShaderRequirements::AddShader(const ShaderCreateInfo& info) {
     m_shaders.PushBack(info);
     return m_shaders.GetSize() - 1;
   }
   
   RenderPassRequirements::RenderPassRequirements(U32 numRenderTargets,
                                                  U32 numPasses,
                                                  U32 numBuffers,
                                                  Memory::Allocator& alloc)
     : m_buffers(numBuffers, alloc),
       m_targets(numRenderTargets, alloc),
       m_passSequence(numPasses, alloc) {
   }
   
   U32 RenderPassRequirements::AddTarget(const RenderTargetCreateInfo& info) {
     m_targets.PushBack(info);
     return m_targets.GetSize() - 1;
   }
   
   U32 RenderPassRequirements::AddBuffer(const StructuredBufferCreateInfo& info) {
     m_buffers.PushBack(info);
     return m_buffers.GetSize() - 1;
   }
   
   U32 RenderPassRequirements::AddPass(const PipelinePassCreateInfo& info) {
     m_passSequence.PushBack(info);
     return m_passSequence.GetSize() - 1;
   }
   
   DescriptorRequirements::DescriptorRequirements(U32 numDescriptors, U32 numSets, Memory::Allocator& alloc)
     : m_descriptorSlots(numDescriptors, alloc),
       m_descriptorSets(numSets, alloc),
       m_allocator(alloc) {
   }
   
   U32 DescriptorRequirements::AddDescriptor(const DescriptorSlotCreateInfo& info) {
     m_descriptorSlots.PushBack(info);
     return m_descriptorSlots.GetSize() - 1;
   }
   
   U32 DescriptorRequirements::AddSet(const std::initializer_list<U32>& sets) {
     m_descriptorSets.EmplaceBack(sets, m_allocator);
     return m_descriptorSets.GetSize() - 1;
   }
   
   } // namespace Azura
