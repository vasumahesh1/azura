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

using namespace Azura::Containers; // NOLINT - Freedom to use using namespace in CPP files.

namespace Azura {
namespace Vulkan {

VkDrawable::VkDrawable(VkDevice device,
                       VkBuffer mainBuffer,
                       const Containers::Vector<VkDescriptorSetLayout>& descriptorSetLayouts,
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
  const Vector<SamplerInfo>& samplerInfos,
  const Vector<VkScopedSampler>& samplers,
  const Vector<VkScopedImage>& images) {
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 2048);

  U32 totalWrites = m_uniformBufferInfos.GetSize() + samplerInfos.GetSize() + images.GetSize();

  Vector<VkWriteDescriptorSet> descriptorWrites(totalWrites, allocatorTemporary);

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

  for (U32 idx = 0; idx < textureBufferInfos.GetSize(); ++idx) {
    const auto& texInfo = textureBufferInfos[idx];
    const auto& scopedImage = images[idx];

    VkDescriptorImageInfo sampledImageInfo = {};
    // TODO(vasumahesh1):[TEXTURE]: Depth Stencil
    sampledImageInfo.imageLayout =  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    sampledImageInfo.imageView = scopedImage.View();

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

  for (U32 idx = 0; idx < samplerInfos.GetSize(); ++idx) {
    const auto& samInfo = samplerInfos[idx];
    const auto& scopedSampler = samplers[idx];

    VkDescriptorImageInfo samplerImageInfo = {};
    samplerImageInfo.sampler = scopedSampler.Real();

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
                               VkRenderPass renderPass,
                               const ApplicationRequirements& appReq,
                               const ViewportDimensions& viewport,
                               const VkPhysicalDeviceMemoryProperties& phyDeviceMemoryProperties,
                               const VkPhysicalDeviceProperties& physicalDeviceProperties,
                               const VkScopedSwapChain& swapChain,
                               Memory::Allocator& allocator,
                               Memory::Allocator& allocatorTemporary,
                               Log logger)
  : DrawablePool(createInfo, allocator),
    m_buffer(device, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | usage, createInfo.m_byteSize, memoryProperties,
             phyDeviceMemoryProperties, logger),
    m_stagingBuffer(device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, createInfo.m_byteSize, memoryProperties,
                    phyDeviceMemoryProperties, logger),
    m_device(device),
    m_renderPass(renderPass),
    m_viewport(viewport),
    m_physicalDeviceMemoryProperties(phyDeviceMemoryProperties),
    m_descriptorSetLayouts(allocator),
    m_pipeline({}),
    m_pipelineLayout(),
    m_pipelineFactory(device, allocatorTemporary, logger),
    m_graphicsCommandPool(graphicsCommandPool),
    m_graphicsQueue(graphicsQueue),
    m_swapChain(swapChain),
    m_appRequirements(appReq),
    m_physicalDeviceProperties(physicalDeviceProperties),
    m_drawables(createInfo.m_numDrawables, allocator),
    m_shaders(createInfo.m_numShaders, allocator),
    m_images(allocator),
    m_samplers(allocator),
    log_VulkanRenderSystem(std::move(logger)) {
  
  CreateDescriptorInfo(createInfo);

  m_samplers.Reserve(m_samplerInfos.GetMaxSize());
}


DrawableID VkDrawablePool::CreateDrawable(const DrawableCreateInfo& createInfo) {
  VkDrawable drawable = VkDrawable(m_device, m_buffer.Real(), m_descriptorSetLayouts, m_descriptorPool, createInfo,
                                   m_numVertexSlots, m_numInstanceSlots, m_numUniformSlots, GetAllocator(),
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

void VkDrawablePool::CreateDescriptorInfo(const DrawablePoolCreateInfo& createInfo) {
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 4096);

  Vector<int> bindingSetSizes{m_descriptorSlots.GetSize(), allocatorTemporary};

  for (const auto& slot : m_descriptorSlots) {
    if (slot.m_binding == DescriptorBinding::Same) {
      bindingSetSizes.Last() += 1;
      continue;
    }

    bindingSetSizes.PushBack(1);
  }

  m_descriptorSetLayouts.Reserve(bindingSetSizes.GetSize());

  int offset = 0;
  for (const auto& bindingSize : bindingSetSizes) {
    Vector<VkDescriptorSetLayoutBinding> currentBindings(bindingSize, allocatorTemporary);

    for (int idx = offset; idx < offset + bindingSize; ++idx) {

      const auto& slot                  = m_descriptorSlots[idx];
      const auto combinedShaderFlagBits = GetCombinedShaderStageFlag(slot.m_stages);

      const auto bindingId = U32(idx - offset);

      switch (slot.m_type) {
        case DescriptorType::UniformBuffer:
          VkCore::CreateUniformBufferBinding(currentBindings, bindingId, 1, combinedShaderFlagBits);
          break;

        case DescriptorType::Sampler:
          VkCore::CreateSamplerBinding(currentBindings, bindingId, 1, combinedShaderFlagBits);
          break;

        case DescriptorType::SampledImage:
          VkCore::CreateSampledImageBinding(currentBindings, bindingId, 1, combinedShaderFlagBits);
          break;

        case DescriptorType::PushConstant:
        case DescriptorType::CombinedImageSampler:
        default:
          LOG_ERR(log_VulkanRenderSystem, LOG_LEVEL, "Unknown Descriptor Type");
          break;
      }
    }

    m_descriptorSetLayouts.
      PushBack(VkCore::CreateDescriptorSetLayout(m_device, currentBindings, log_VulkanRenderSystem));

    offset += bindingSize;
  }

  m_pipelineLayout = VkCore::CreatePipelineLayout(m_device, m_descriptorSetLayouts, log_VulkanRenderSystem);

  Vector<VkDescriptorPoolSize> descriptorPoolSizes(MAX_DESCRIPTOR_TYPE_COUNT, allocatorTemporary);

  // TODO(vasumahesh1):[DESCRIPTOR]: How to use Uniform Buffer Arrays?
  VkDescriptorPoolSize uniformPoolSize = {};
  uniformPoolSize.type                 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uniformPoolSize.descriptorCount      = m_numUniformSlots;
  descriptorPoolSizes.PushBack(uniformPoolSize);

  if (m_numSamplerSlots > 0) {
    VkDescriptorPoolSize samplerPoolSize = {};
    samplerPoolSize.type                 = VK_DESCRIPTOR_TYPE_SAMPLER;
    samplerPoolSize.descriptorCount      = m_numSamplerSlots;
    descriptorPoolSizes.PushBack(samplerPoolSize);
  }

  if (m_numSampledImageSlots > 0) {
    VkDescriptorPoolSize sampledImagePoolSize = {};
    sampledImagePoolSize.type                 = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    sampledImagePoolSize.descriptorCount      = m_numSampledImageSlots;
    descriptorPoolSizes.PushBack(sampledImagePoolSize);
  }

  // TODO(vasumahesh1):[DESCRIPTOR]: 1 Set per Drawable? Need to Check
  VkDescriptorPoolCreateInfo poolInfo = {};
  poolInfo.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount              = descriptorPoolSizes.GetSize();
  poolInfo.pPoolSizes                 = descriptorPoolSizes.Data();
  poolInfo.maxSets                    = createInfo.m_numDrawables * m_descriptorSetLayouts.GetSize();

  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool),
    "Unable to create Descriptor Pool");
}


void VkDrawablePool::SubmitTextureData() {
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

  for(auto& image : m_images)
  {
    // TODO(vasumahesh1):[TEXTURE]: Remove Hard Code
    image.CreateImageView(ImageViewType::ImageView2D);
  }

  for (auto& drawable : m_drawables) {
    drawable.WriteDescriptorSets(m_textureBufferInfos, m_samplerInfos, m_samplers, m_images);
  }
}

void VkDrawablePool::Submit() {
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 4096);

  m_pipelineFactory.SetInputAssemblyStage(PrimitiveTopology::TriangleList);
  m_pipelineFactory.SetRasterizerStage(m_cullMode, FrontFace::CounterClockwise);
  m_pipelineFactory.SetPipelineLayout(m_pipelineLayout);
  m_pipelineFactory.SetRenderPass(m_renderPass);
  m_pipelineFactory.SetViewportStage(m_viewport, m_swapChain);
  m_pipelineFactory.SetMultisampleStage();
  m_pipelineFactory.SetColorBlendStage();

  m_pipeline = m_pipelineFactory.Submit();

  m_commandBuffer = VkCore::CreateCommandBuffer(m_device, m_graphicsCommandPool, VK_COMMAND_BUFFER_LEVEL_SECONDARY,
                                                log_VulkanRenderSystem);

  VkCommandBufferInheritanceInfo inheritanceInfo = {};
  inheritanceInfo.sType                          = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
  inheritanceInfo.renderPass                     = m_renderPass;
  inheritanceInfo.framebuffer                    = VK_NULL_HANDLE;

  VkCore::BeginCommandBuffer(m_commandBuffer,
                             VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT |
                             VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, inheritanceInfo, log_VulkanRenderSystem);

  vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.Real());

  VkCore::CopyBuffer(m_device, m_graphicsQueue, m_stagingBuffer, m_buffer, m_mainBufferOffset, m_graphicsCommandPool);

  SubmitTextureData();

  VkBuffer mainBuffer = m_buffer.Real();

  for (auto& drawable : m_drawables) {
    const auto& descriptorSets = drawable.GetDescriptorSet();

    const auto& vertBufferInfos = drawable.GetVertexBufferInfos();

    for (const auto& vertexBuffer : vertBufferInfos) {
      VkDeviceSize offsets[] = {vertexBuffer.m_offset};
      vkCmdBindVertexBuffers(m_commandBuffer, vertexBuffer.m_binding, 1, &mainBuffer, &offsets[0]);
    }

    const auto& instanceBufferInfos = drawable.GetInstanceBufferInfos();

    for (const auto& instanceBuffer : instanceBufferInfos) {
      VkDeviceSize offsets[] = {instanceBuffer.m_offset};
      vkCmdBindVertexBuffers(m_commandBuffer, instanceBuffer.m_binding, 1, &mainBuffer, &offsets[0]);
    }

    const auto& indexBufferInfo = drawable.GetIndexBufferInfo();

    const auto indexType = ToVkIndexType(drawable.GetIndexType());
    VERIFY_OPT(log_VulkanRenderSystem, indexType, "Invalid VkIndexType converted");

    vkCmdBindIndexBuffer(m_commandBuffer, mainBuffer, indexBufferInfo.m_offset, indexType.value());

    vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0,
                            descriptorSets.GetSize(), descriptorSets.Data(), 0, nullptr);

    switch (GetDrawType()) {
      case DrawType::InstancedIndexed:
        vkCmdDrawIndexed(m_commandBuffer, drawable.GetIndexCount(), drawable.GetInstanceCount(), 0, 0, 0);
        break;

      case DrawType::InstancedIndexedIndirect:
        break;

      default:
        break;
    }
  }

  VkCore::EndCommandBuffer(m_commandBuffer, log_VulkanRenderSystem);
}

void VkDrawablePool::AddBufferBinding(SlotID slotId, const Containers::Vector<RawStorageFormat>& strides) {
  const int idx = GetVertexSlotIndex(slotId);

  if (idx < 0) {
    LOG_ERR(log_VulkanRenderSystem, LOG_LEVEL, "Invalid Slot ID for Buffer Binding: %d", slotId);
    return;
  }

  m_pipelineFactory.BulkAddAttributeDescription(strides, idx);

  U32 totalBufferStride = 0;

  for (const auto& stride : strides) {
    totalBufferStride += GetFormatSize(stride);
  }

  m_pipelineFactory.AddBindingDescription(totalBufferStride, m_vertexDataSlots[idx], idx);
}

void VkDrawablePool::AddShader(const Shader& shader) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
  auto vkShader = static_cast<const VkShader&>(shader);

  m_shaders.PushBack(vkShader);
  m_pipelineFactory.AddShaderStage(m_shaders[m_shaders.GetSize() - 1].GetShaderStageInfo());
}

void VkDrawablePool::CleanUp() const {
  m_buffer.CleanUp();
  m_stagingBuffer.CleanUp();

  m_pipeline.CleanUp(m_device);

  vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);

  vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);

  for (const auto& image : m_images) {
    image.CleanUp();
  }

  for (const auto& sampler : m_samplers) {
    sampler.CleanUp();
  }

  for (const auto& setLayout : m_descriptorSetLayouts) {
    vkDestroyDescriptorSetLayout(m_device, setLayout, nullptr);
  }

  vkFreeCommandBuffers(m_device, m_graphicsCommandPool, 1, &m_commandBuffer);

  for (const auto& shader : m_shaders) {
    shader.CleanUp(m_device);
  }
}

const VkCommandBuffer& VkDrawablePool::GetCommandBuffer() const {
  return m_commandBuffer;
}

void VkDrawablePool::BindVertexData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) {
  assert(m_drawables.GetSize() > drawableId);

  auto& drawable = m_drawables[drawableId];

  const int idx = GetVertexSlotIndex(slot);

  if (idx < 0) {
    LOG_ERR(log_VulkanRenderSystem, LOG_LEVEL, "Invalid Slot ID for Binding Vertex Data: %d", slot);
    return;
  }

  BufferInfo info    = BufferInfo();
  info.m_maxByteSize = size;
  info.m_byteSize    = size;
  info.m_offset      = m_mainBufferOffset;
  info.m_binding     = idx;

  drawable.AddVertexBufferInfo(std::move(info));
  AppendToMainBuffer(buffer, size);
}

void VkDrawablePool::BindInstanceData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) {
  assert(m_drawables.GetSize() > drawableId);

  auto& drawable = m_drawables[drawableId];

  const int idx = GetVertexSlotIndex(slot);

  if (idx < 0) {
    LOG_ERR(log_VulkanRenderSystem, LOG_LEVEL, "Invalid Slot ID for Binding Instance Data: %d", slot);
    return;
  }

  BufferInfo info    = BufferInfo();
  info.m_maxByteSize = size;
  info.m_byteSize    = size;
  info.m_offset      = m_mainBufferOffset;
  info.m_binding     = idx;

  drawable.AddInstanceBufferInfo(std::move(info));
  AppendToMainBuffer(buffer, size);
}

void VkDrawablePool::BindUniformData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) {
  const int idx = GetDescriptorSlotIndex(slot);

  if (idx < 0) {
    LOG_ERR(log_VulkanRenderSystem, LOG_LEVEL, "Invalid Slot ID for Binding Descriptor Data: %d", slot);
    return;
  }

  const auto minAlignment = U32(m_physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);

  assert(m_drawables.GetSize() > drawableId);

  if (minAlignment > 0) {
    size = (size + minAlignment - 1) & ~(minAlignment - 1);
  }

  auto& drawable = m_drawables[drawableId];

  const U32 currentOffset = m_mainBufferOffset;
  const U32 newOffset     = (currentOffset + minAlignment - 1) & ~(minAlignment - 1);
  m_mainBufferOffset += newOffset - currentOffset;

  const auto& descriptorSlot = m_descriptorSlots[idx];

  UniformBufferInfo info = UniformBufferInfo();
  info.m_byteSize        = size;
  info.m_offset          = newOffset;
  info.m_binding         = descriptorSlot.m_bindIdx;
  info.m_set             = descriptorSlot.m_setIdx;

  drawable.AddUniformBufferInfo(std::move(info));
  AppendToMainBuffer(buffer, size);
}

void VkDrawablePool::BindTextureData(SlotID slot, const TextureDesc& desc, const U8* buffer) {
  const int idx = GetDescriptorSlotIndex(slot);

  if (idx < 0) {
    LOG_ERR(log_VulkanRenderSystem, LOG_LEVEL, "Invalid Slot ID for Binding Texture Descriptor Data: %d", slot);
    return;
  }

  const U32 size = desc.m_size;

  const auto& descriptorSlot = m_descriptorSlots[idx];

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
  const int idx = GetDescriptorSlotIndex(slot);

  if (idx < 0) {
    LOG_ERR(log_VulkanRenderSystem, LOG_LEVEL, "Invalid Slot ID for Binding Texture Descriptor Data: %d", slot);
    return;
  }

  const auto& descriptorSlot = m_descriptorSlots[idx];

  if (descriptorSlot.m_type != DescriptorType::Sampler)
  {
    LOG_ERR(log_VulkanRenderSystem, LOG_LEVEL, "Slot is not a Sampler: %d", slot);
    return;
  }

  SamplerInfo sInfo = {};
  sInfo.m_set = descriptorSlot.m_setIdx;
  sInfo.m_binding = descriptorSlot.m_bindIdx;

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
