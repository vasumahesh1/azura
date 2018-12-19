
.. _program_listing_file_Source_Azura_RenderSystem_Src_Vulkan_VkDrawablePool.cpp:

Program Listing for File VkDrawablePool.cpp
===========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Src_Vulkan_VkDrawablePool.cpp>` (``Source\Azura\RenderSystem\Src\Vulkan\VkDrawablePool.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Vulkan/VkDrawablePool.h"
   #include "Memory/MemoryFactory.h"
   #include "Vulkan/VkMacros.h"
   #include "Containers/Vector.h"
   #include "Memory/MonotonicAllocator.h"
   #include "Generic/Renderer.h"
   #include "Generic/TextureManager.h"
   #include "Vulkan/VkCore.h"
   #include "Vulkan/VkTypeMapping.h"
   #include "Core/RawStorageFormat.h"
   #include "Vulkan/VkScopedRenderPass.h"
   
   using namespace Azura::Containers; // NOLINT - Freedom to use using namespace in CPP files.
   
   namespace Azura {
   namespace Vulkan {
   
   VkDrawable::VkDrawable(VkDevice device,
                          VkBuffer mainBuffer,
                          const Vector<VkDescriptorSetLayout>& descriptorSetLayouts,
                          VkDescriptorPool descriptorPool,
                          const DrawableCreateInfo& info,
                          U32 numVertexSlots,
                          U32 numInstanceSlots,
                          U32 numUniformSlots,
                          Memory::Allocator& allocator,
                          Log logger)
     : Drawable(info, numVertexSlots, numInstanceSlots, numUniformSlots, allocator),
       m_device(device),
       m_descriptorSetLayouts(descriptorSetLayouts, allocator),
       m_descriptorPool(descriptorPool),
       m_mainBuffer(mainBuffer),
       m_descriptorSets(ContainerExtent{descriptorSetLayouts.GetSize()}, allocator),
       log_VulkanRenderSystem(std::move(logger)) {
   
     // Allocate the Descriptor Sets
     VkDescriptorSetAllocateInfo allocInfo = {};
     allocInfo.sType                       = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
     allocInfo.descriptorPool              = m_descriptorPool;
     allocInfo.descriptorSetCount          = m_descriptorSetLayouts.GetSize();
     allocInfo.pSetLayouts                 = m_descriptorSetLayouts.Data();
   
     VERIFY_VK_OP(log_VulkanRenderSystem, vkAllocateDescriptorSets(m_device, &allocInfo, m_descriptorSets.Data()),
       "Failed to Allocate Descriptor Set");
   }
   
   void VkDrawable::WriteDescriptorSets(
     const Vector<TextureBufferInfo>& textureBufferInfos,
     const Vector<std::reference_wrapper<VkScopedRenderPass>>& renderPasses,
     const Vector<SamplerInfo>& samplerInfos,
     const Vector<VkScopedSampler>& samplers,
     const Vector<VkScopedImage>& images,
     const Vector<VkScopedImage>& renderPassAttachments) {
     STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 2048);
   
     // TODO(vasumahesh1):[DESCRIPTOR]: How to use Uniform Buffer Arrays?
     for (const auto& ubInfo : m_uniformBufferInfos) {
       VkDescriptorBufferInfo uniformBufferInfo = {};
       uniformBufferInfo.buffer                 = m_mainBuffer;
       uniformBufferInfo.offset                 = ubInfo.m_offset;
       uniformBufferInfo.range                  = ubInfo.m_byteSize;
   
       VkWriteDescriptorSet descriptorWrite = {};
       descriptorWrite.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
       descriptorWrite.dstSet               = m_descriptorSets[ubInfo.m_set];
       descriptorWrite.dstBinding           = ubInfo.m_binding;
       descriptorWrite.dstArrayElement      = 0;
       descriptorWrite.descriptorType       = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
       descriptorWrite.descriptorCount      = 1;
       descriptorWrite.pBufferInfo          = &uniformBufferInfo;
       descriptorWrite.pImageInfo           = nullptr;
       descriptorWrite.pTexelBufferView     = nullptr;
   
       vkUpdateDescriptorSets(m_device, 1, &descriptorWrite, 0, nullptr);
     }
   
     for (U32 idx              = 0; idx < textureBufferInfos.GetSize(); ++idx) {
       const auto& texInfo     = textureBufferInfos[idx];
       const auto& scopedImage = images[idx];
   
       VkDescriptorImageInfo sampledImageInfo = {};
       // TODO(vasumahesh1):[TEXTURE]: Depth Stencil
       sampledImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
       sampledImageInfo.imageView   = scopedImage.View();
   
       VkWriteDescriptorSet descriptorWrite = {};
       descriptorWrite.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
       descriptorWrite.dstSet               = m_descriptorSets[texInfo.m_set];
       descriptorWrite.dstBinding           = texInfo.m_binding;
       descriptorWrite.dstArrayElement      = 0;
       descriptorWrite.descriptorType       = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
       descriptorWrite.descriptorCount      = 1;
       descriptorWrite.pBufferInfo          = nullptr;
       descriptorWrite.pImageInfo           = &sampledImageInfo;
       descriptorWrite.pTexelBufferView     = nullptr;
   
       vkUpdateDescriptorSets(m_device, 1, &descriptorWrite, 0, nullptr);
     }
   
     for (U32 idx                = 0; idx < samplerInfos.GetSize(); ++idx) {
       const auto& samInfo       = samplerInfos[idx];
       const auto& scopedSampler = samplers[idx];
   
       VkDescriptorImageInfo samplerImageInfo = {};
       samplerImageInfo.sampler               = scopedSampler.Real();
   
       VkWriteDescriptorSet descriptorWrite = {};
       descriptorWrite.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
       descriptorWrite.dstSet               = m_descriptorSets[samInfo.m_set];
       descriptorWrite.dstBinding           = samInfo.m_binding;
       descriptorWrite.dstArrayElement      = 0;
       descriptorWrite.descriptorType       = VK_DESCRIPTOR_TYPE_SAMPLER;
       descriptorWrite.descriptorCount      = 1;
       descriptorWrite.pBufferInfo          = nullptr;
       descriptorWrite.pImageInfo           = &samplerImageInfo;
       descriptorWrite.pTexelBufferView     = nullptr;
   
       vkUpdateDescriptorSets(m_device, 1, &descriptorWrite, 0, nullptr);
     }
   
     for (const auto& renderPass : renderPasses)
     {
       const auto& inputs = renderPass.get().GetPassInputs();
   
       // No Inputs then No need to write the descriptor set
       if (inputs.GetSize() == 0)
       {
         continue;
       }
   
       const auto setId = renderPass.get().GetDescriptorSetId();
       LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Input Attachment: Set: %d", setId);
   
       Vector<VkDescriptorImageInfo> descriptorSetInfo(inputs.GetSize(), allocatorTemporary);
   
       for (U32 idx = 0; idx < inputs.GetSize(); ++idx) {
         const auto& inputInfo = inputs[idx];
   
         LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Input Attachment: Set: %d Binding: %d Input ID: %d", setId, descriptorSetInfo.GetSize(), inputInfo.m_id);
   
         VkDescriptorImageInfo sampledImageInfo = {};
         // TODO(vasumahesh1):[TEXTURE]: Depth Stencil
         sampledImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
         sampledImageInfo.imageView   = renderPassAttachments[inputInfo.m_id].View();
   
         descriptorSetInfo.PushBack(sampledImageInfo);
       }
   
       VkWriteDescriptorSet descriptorWrite = {};
       descriptorWrite.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
       descriptorWrite.dstSet               = m_descriptorSets[setId];
       descriptorWrite.dstBinding           = 0;
       descriptorWrite.dstArrayElement      = 0;
       descriptorWrite.descriptorType       = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
       descriptorWrite.descriptorCount      = descriptorSetInfo.GetSize();
       descriptorWrite.pBufferInfo          = nullptr;
       descriptorWrite.pImageInfo           = descriptorSetInfo.Data();
       descriptorWrite.pTexelBufferView     = nullptr;
   
       vkUpdateDescriptorSets(m_device, 1, &descriptorWrite, 0, nullptr);
     }
   }
   
   const Containers::Vector<VkDescriptorSet>& VkDrawable::GetDescriptorSet() const {
     return m_descriptorSets;
   }
   
   void VkDrawable::CleanUp(VkDevice device) const {
     UNUSED(device);
   }
   
   VkDrawablePool::VkDrawablePool(const DrawablePoolCreateInfo& createInfo,
                                  VkDevice device,
                                  VkQueue graphicsQueue,
                                  VkBufferUsageFlags usage,
                                  VkMemoryPropertyFlags memoryProperties,
                                  VkCommandPool graphicsCommandPool,
                                  VkPipelineLayout pipelineLayout,
                                  VkDescriptorPool descriptorPool,
                                  const Vector<VkDescriptorSetLayout>& descriptorSetLayouts,
                                  const Vector<VkScopedRenderPass>& renderPasses,
                                  const Vector<VkScopedImage>& renderPassAttachments,
                                  const Vector<VkShader>& allShaders,
                                  const ApplicationRequirements& appReq,
                                  const ViewportDimensions& viewport,
                                  const VkPhysicalDeviceMemoryProperties& phyDeviceMemoryProperties,
                                  const VkPhysicalDeviceProperties& physicalDeviceProperties,
                                  const VkScopedSwapChain& swapChain,
                                  const Containers::Vector<DescriptorSlot>& descriptorSlots,
                                  const DescriptorCount& descriptorCount,
                                  Memory::Allocator& allocator,
                                  Memory::Allocator& allocatorTemporary,
                                  Log logger)
     : DrawablePool(createInfo, descriptorCount, allocator),
       m_buffer(device, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | usage, createInfo.m_byteSize,
                memoryProperties,
                phyDeviceMemoryProperties, logger),
       m_stagingBuffer(device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, createInfo.m_byteSize, memoryProperties,
                       phyDeviceMemoryProperties, logger),
       m_device(device),
       m_renderPasses(renderPasses.GetSize(), allocator),
       m_viewport(viewport),
       m_descriptorSetLayouts(descriptorSetLayouts),
       m_descriptorSlots(descriptorSlots),
       m_renderPassAttachments(renderPassAttachments),
       m_allShaders(allShaders),
       m_physicalDeviceMemoryProperties(phyDeviceMemoryProperties),
       m_descriptorPool(descriptorPool),
       m_pipelines(allocator),
       m_pipelineLayout(pipelineLayout),
       m_pipelineFactory(device, allocatorTemporary, logger),
       m_commandBuffers(allocator),
       m_graphicsCommandPool(graphicsCommandPool),
       m_graphicsQueue(graphicsQueue),
       m_swapChain(swapChain),
       m_appRequirements(appReq),
       m_physicalDeviceProperties(physicalDeviceProperties),
       m_drawables(createInfo.m_numDrawables, allocator),
       m_images(allocator),
       m_samplers(allocator),
       log_VulkanRenderSystem(std::move(logger)) {
     m_samplers.Reserve(m_samplerInfos.GetMaxSize());
   
     U32 idx = 0;
     for(const auto& renderPass : renderPasses)
     {
       auto it = std::find_if(createInfo.m_renderPasses.Begin(), createInfo.m_renderPasses.End(), [&](U32 passId)
       {
         return renderPass.GetId() == passId;
       });
   
       if (it != createInfo.m_renderPasses.End())
       {
         m_renderPasses.PushBack(std::reference_wrapper<VkScopedRenderPass>(renderPasses[idx]));
       }
   
       ++idx;
     }
   
     idx = 0;
     for(const auto& vertexSlot : createInfo.m_vertexDataSlots)
     {
       m_pipelineFactory.BulkAddAttributeDescription(vertexSlot, idx);
   
       U32 totalBufferStride = 0;
   
       for (const auto& semanticStride : vertexSlot.m_stride) {
         totalBufferStride += GetFormatSize(semanticStride.m_format);
       }
   
       m_pipelineFactory.AddBindingDescription(totalBufferStride, m_vertexDataSlots[idx], idx);
   
       ++idx;
     }
   }
   
   void VkDrawablePool::AddShader(const U32 shaderId) {
     m_pipelineFactory.AddShaderStage(m_allShaders[shaderId ].GetShaderStageInfo());
   }
   
   
   DrawableID VkDrawablePool::CreateDrawable(const DrawableCreateInfo& createInfo) {
     VkDrawable drawable = VkDrawable(m_device, m_buffer.Real(), m_descriptorSetLayouts, m_descriptorPool, createInfo,
                                      m_numVertexSlots, m_numInstanceSlots, m_descriptorCount.m_numUniformSlots,
                                      GetAllocator(),
                                      log_VulkanRenderSystem);
     m_drawables.PushBack(std::move(drawable));
     return m_drawables.GetSize() - 1;
   }
   
   void VkDrawablePool::AppendToMainBuffer(const U8* buffer, U32 bufferSize) {
     void* data = m_stagingBuffer.MapMemory(bufferSize, m_mainBufferOffset);
     std::memcpy(data, buffer, bufferSize);
     m_stagingBuffer.UnMapMemory();
   
     // Record Offset Changes
     m_mainBufferOffset += bufferSize;
   }
   
   void VkDrawablePool::BeginUpdates() {
   }
   
   void VkDrawablePool::UpdateUniformData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) {
     UNUSED(drawableId);
     UNUSED(slot);
     UNUSED(buffer);
     UNUSED(size);
   }
   
   void VkDrawablePool::UpdateVertexData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) {
     UNUSED(drawableId);
     UNUSED(slot);
     UNUSED(buffer);
     UNUSED(size);
   }
   
   void VkDrawablePool::UpdateInstanceData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) {
     UNUSED(drawableId);
     UNUSED(slot);
     UNUSED(buffer);
     UNUSED(size);
   }
   
   void VkDrawablePool::UpdateTextureData(SlotID slot, const U8* buffer) {
     UNUSED(slot);
     UNUSED(buffer);
   }
   
   void VkDrawablePool::SubmitUpdates() {
   }
   
   
   void VkDrawablePool::SubmitTextureData() {
     if (m_textureBufferInfos.GetSize() == 0)
     {
       return;
     }
   
     STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 4096);
   
     VkCommandBuffer textureCmdBuffer = VkCore::CreateCommandBuffer(m_device, m_graphicsCommandPool,
                                                                    VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                                                    log_VulkanRenderSystem);
     VkCore::BeginCommandBuffer(textureCmdBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, log_VulkanRenderSystem);
   
     const VkBuffer mainBuffer = m_buffer.Real();
   
     m_images.Reserve(m_textureBufferInfos.GetSize());
   
     const VkImageUsageFlags textureUsage = (VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                             VK_IMAGE_USAGE_SAMPLED_BIT);
   
     const ImageTransition layoutUndefined{
       VK_IMAGE_LAYOUT_UNDEFINED, 0, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
     };
   
     const ImageTransition layoutDst{
       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT
     };
   
     const ImageTransition layoutShaderRead{
       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
     };
   
     for (const auto& textureInfo : m_textureBufferInfos) {
       VkScopedImage image = VkScopedImage(m_device, textureInfo.m_desc, textureUsage, m_physicalDeviceMemoryProperties,
                                           log_VulkanRenderSystem);
       image.TransitionLayout(textureCmdBuffer, layoutUndefined, layoutDst);
       image.CopyFromBuffer(textureCmdBuffer, textureInfo, mainBuffer);
       image.TransitionLayout(textureCmdBuffer, layoutDst, layoutShaderRead);
       m_images.PushBack(image);
     }
   
     // Flush & Wait
     VkCore::FlushCommandBuffer(m_device, textureCmdBuffer, m_graphicsQueue, log_VulkanRenderSystem);
     vkFreeCommandBuffers(m_device, m_graphicsCommandPool, 1, &textureCmdBuffer);
   
     for (auto& image : m_images) {
       // TODO(vasumahesh1):[TEXTURE]: Remove Hard Code
       image.CreateImageView(ImageViewType::ImageView2D);
     }
   }
   
   void VkDrawablePool::Submit() {
     STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 4096);
   
     m_pipelineFactory.SetInputAssemblyStage(PrimitiveTopology::TriangleList);
     m_pipelineFactory.SetRasterizerStage(m_cullMode, FrontFace::CounterClockwise);
     m_pipelineFactory.SetPipelineLayout(m_pipelineLayout);
     m_pipelineFactory.SetViewportStage(m_viewport, m_swapChain);
     m_pipelineFactory.SetMultisampleStage();
   
     m_pipelineFactory.Submit(m_renderPasses, m_pipelines);
   
     m_commandBuffers.Resize(m_renderPasses.GetSize());
     VkCore::CreateCommandBuffers(m_device, m_graphicsCommandPool, VK_COMMAND_BUFFER_LEVEL_SECONDARY, m_commandBuffers,
                                  log_VulkanRenderSystem);
   
     // Flush Entire Staging Buffer to GPU
     UNUSED(m_stagingBuffer.MapMemory(m_mainBufferOffset, 0));
     VkMappedMemoryRange currentRange = {};
     currentRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
     currentRange.memory = m_stagingBuffer.Memory();
     currentRange.size = VK_WHOLE_SIZE;
     currentRange.offset = 0;
     vkFlushMappedMemoryRanges(m_device, 1, &currentRange);
     m_stagingBuffer.UnMapMemory();
   
     VkCore::CopyBuffer(m_device, m_graphicsQueue, m_stagingBuffer, m_buffer, m_mainBufferOffset, m_graphicsCommandPool);
   
     SubmitTextureData();
   
     for (auto& drawable : m_drawables) {
       drawable.WriteDescriptorSets(m_textureBufferInfos, m_renderPasses, m_samplerInfos, m_samplers, m_images, m_renderPassAttachments);
     }
   
     U32 count = 0;
     for (const auto& renderPass : m_renderPasses) {
       const auto& commandBuffer = m_commandBuffers[count];
       const auto& pipeline      = m_pipelines[count];
   
       VkCommandBufferInheritanceInfo inheritanceInfo = {};
       inheritanceInfo.sType                          = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
       inheritanceInfo.renderPass                     = renderPass.get().GetRenderPass();
       inheritanceInfo.framebuffer                    = VK_NULL_HANDLE;
       // TODO(vasumahesh1):[PERF]: renderPass.GetFrameBuffer();
   
       VkCore::BeginCommandBuffer(commandBuffer,
                                  VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT |
                                  VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, inheritanceInfo,
                                  log_VulkanRenderSystem);
   
       vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.Real());
   
       VkBuffer mainBuffer = m_buffer.Real();
   
       for (auto& drawable : m_drawables) {
         const auto& descriptorSets = drawable.GetDescriptorSet();
   
         const auto& vertBufferInfos = drawable.GetVertexBufferInfos();
   
         for (const auto& vertexBuffer : vertBufferInfos) {
           VkDeviceSize offsets[] = {vertexBuffer.m_offset};
           vkCmdBindVertexBuffers(commandBuffer, vertexBuffer.m_binding, 1, &mainBuffer, &offsets[0]);
         }
   
         const auto& instanceBufferInfos = drawable.GetInstanceBufferInfos();
   
         for (const auto& instanceBuffer : instanceBufferInfos) {
           VkDeviceSize offsets[] = {instanceBuffer.m_offset};
           vkCmdBindVertexBuffers(commandBuffer, instanceBuffer.m_binding, 1, &mainBuffer, &offsets[0]);
         }
   
         const auto& indexBufferInfo = drawable.GetIndexBufferInfo();
   
         const auto indexType = ToVkIndexType(drawable.GetIndexType());
         VERIFY_OPT(log_VulkanRenderSystem, indexType, "Invalid VkIndexType converted");
   
         vkCmdBindIndexBuffer(commandBuffer, mainBuffer, indexBufferInfo.m_offset, indexType.value());
   
         vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0,
                                 descriptorSets.GetSize(), descriptorSets.Data(), 0, nullptr);
   
         switch (GetDrawType()) {
           case DrawType::InstancedIndexed:
             vkCmdDrawIndexed(commandBuffer, drawable.GetIndexCount(), drawable.GetInstanceCount(), 0, 0, 0);
             break;
   
           case DrawType::InstancedIndexedIndirect:
             break;
   
           default:
             break;
         }
       }
   
       VkCore::EndCommandBuffer(commandBuffer, log_VulkanRenderSystem);
   
       ++count;
     }
   }
   
   void VkDrawablePool::CleanUp() const {
     m_buffer.CleanUp();
     m_stagingBuffer.CleanUp();
   
     for (auto& pipeline : m_pipelines) {
       pipeline.CleanUp(m_device);
     }
   
     for (const auto& image : m_images) {
       image.CleanUp();
     }
   
     for (const auto& sampler : m_samplers) {
       sampler.CleanUp();
     }
   
     vkFreeCommandBuffers(m_device, m_graphicsCommandPool, m_commandBuffers.GetSize(), m_commandBuffers.Data());
   }
   
   void VkDrawablePool::GetCommandBuffers(Vector<std::pair<U32, VkCommandBuffer>>& commandBuffers) const {
     U32 idx = 0;
     commandBuffers.Reserve(m_renderPasses.GetSize());
   
     for (const auto& renderPass : m_renderPasses) {
       commandBuffers.PushBack(std::make_pair(renderPass.get().GetId(), m_commandBuffers[idx]));
       ++idx;
     }
   }
   
   void VkDrawablePool::BindVertexData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) {
     assert(m_drawables.GetSize() > drawableId);
   
     auto& drawable = m_drawables[drawableId];
   
     BufferInfo info    = BufferInfo();
     info.m_maxByteSize = size;
     info.m_byteSize    = size;
     info.m_offset      = m_mainBufferOffset;
     info.m_binding     = slot;
   
     drawable.AddVertexBufferInfo(std::move(info));
     AppendToMainBuffer(buffer, size);
   }
   
   void VkDrawablePool::BindVertexData(DrawableID drawableId, SlotID slot, U32 sourceBuffer, U32 offset, U32 size) {
     UNUSED(drawableId);
     UNUSED(slot);
     UNUSED(sourceBuffer);
     UNUSED(offset);
     UNUSED(size);
   }
   
   void VkDrawablePool::BindInstanceData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) {
     assert(m_drawables.GetSize() > drawableId);
   
     auto& drawable = m_drawables[drawableId];
   
     BufferInfo info    = BufferInfo();
     info.m_maxByteSize = size;
     info.m_byteSize    = size;
     info.m_offset      = m_mainBufferOffset;
     info.m_binding     = slot;
   
     drawable.AddInstanceBufferInfo(std::move(info));
     AppendToMainBuffer(buffer, size);
   }
   
   void VkDrawablePool::BindUniformData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) {
     const auto minAlignment = U32(m_physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
   
     assert(m_drawables.GetSize() > drawableId);
   
     if (minAlignment > 0) {
       size = (size + minAlignment - 1) & ~(minAlignment - 1);
     }
   
     auto& drawable = m_drawables[drawableId];
   
     const U32 currentOffset = m_mainBufferOffset;
     const U32 newOffset     = (currentOffset + minAlignment - 1) & ~(minAlignment - 1);
     m_mainBufferOffset += newOffset - currentOffset;
   
     const auto& descriptorSlot = m_descriptorSlots[slot];
   
     UniformBufferInfo info = UniformBufferInfo();
     info.m_byteSize        = size;
     info.m_offset          = newOffset;
     info.m_binding         = descriptorSlot.m_bindIdx;
     info.m_set             = descriptorSlot.m_setIdx;
   
     drawable.AddUniformBufferInfo(std::move(info));
     AppendToMainBuffer(buffer, size);
   }
   
   void VkDrawablePool::BindTextureData(SlotID slot, const TextureDesc& desc, const U8* buffer) {
     const U32 size = desc.m_size;
   
     const auto& descriptorSlot = m_descriptorSlots[slot];
   
     TextureBufferInfo info = TextureBufferInfo();
     info.m_byteSize        = size;
     info.m_offset          = m_mainBufferOffset;
     info.m_desc            = desc;
     info.m_binding         = descriptorSlot.m_bindIdx;
     info.m_set             = descriptorSlot.m_setIdx;
   
     m_textureBufferInfos.PushBack(info);
     AppendToMainBuffer(buffer, size);
   }
   
   void VkDrawablePool::BindSampler(SlotID slot, const SamplerDesc& desc) {
     UNUSED(desc);
   
     const auto& descriptorSlot = m_descriptorSlots[slot];
   
     if (descriptorSlot.m_type != DescriptorType::Sampler) {
       LOG_ERR(log_VulkanRenderSystem, LOG_LEVEL, "Slot is not a Sampler: %d", slot);
       return;
     }
   
     SamplerInfo sInfo = {};
     sInfo.m_set       = descriptorSlot.m_setIdx;
     sInfo.m_binding   = descriptorSlot.m_bindIdx;
   
     m_samplerInfos.PushBack(sInfo);
     m_samplers.PushBack(VkScopedSampler(m_device, log_VulkanRenderSystem));
   }
   
   void VkDrawablePool::SetIndexData(DrawableID drawableId, const U8* buffer, U32 size) {
     auto& drawable = m_drawables[drawableId];
   
     BufferInfo info = BufferInfo();
     info.m_byteSize = size;
     info.m_offset   = m_mainBufferOffset;
   
     drawable.SetIndexBufferInfo(std::move(info));
     AppendToMainBuffer(buffer, size);
   }
   
   } // namespace Vulkan
   } // namespace Azura
