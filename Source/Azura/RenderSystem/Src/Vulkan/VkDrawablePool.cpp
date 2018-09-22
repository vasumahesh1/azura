#include "Vulkan/VkDrawablePool.h"
#include "Memory/MemoryFactory.h"
#include "Vulkan/VkMacros.h"
#include "Containers/Vector.h"
#include "Memory/MonotonicAllocator.h"
#include "Generic/Renderer.h"
#include "Vulkan/VkCore.h"
#include "Vulkan/VkTypeMapping.h"
#include "Core/RawStorageFormat.h"

using namespace Azura::Containers; // NOLINT - Freedom to use using namespace in CPP files.

namespace Azura {
namespace Vulkan {

VkDrawable::VkDrawable(VkDevice device,
                       VkBuffer mainBuffer,
                       VkDescriptorSetLayout descriptorSetLayout,
                       VkDescriptorPool descriptorPool,
                       const DrawableCreateInfo& info,
  U32 numVertexSlots, U32 numInstanceSlots, U32 numUniformSlots,
                       Memory::Allocator& allocator,
                       Log logger)
  : Drawable(info, numVertexSlots, numInstanceSlots, numUniformSlots, allocator),
    m_device(device),
    m_descriptorSetLayout(descriptorSetLayout),
    m_descriptorPool(descriptorPool),
    m_mainBuffer(mainBuffer),
    m_descriptorSet(),
    log_VulkanRenderSystem(std::move(logger)) {
}

void VkDrawable::Submit() {
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 2048);

  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType                       = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool              = m_descriptorPool;
  allocInfo.descriptorSetCount          = 1;
  allocInfo.pSetLayouts                 = &m_descriptorSetLayout;

  VERIFY_VK_OP(log_VulkanRenderSystem, vkAllocateDescriptorSets(m_device, &allocInfo, &m_descriptorSet),
    "Failed to Allocate Descriptor Set");

  Vector<VkWriteDescriptorSet> descriptorWrites(m_uniformBufferInfos.GetSize(), allocatorTemporary);

  // TODO(vasumahesh1):[DESCRIPTOR]: How to use Uniform Buffer Arrays?
  // TODO(vasumahesh1):[TEXTURE]: Add support for Textures

  for (const auto& ubInfo : m_uniformBufferInfos) {
    VkDescriptorBufferInfo uniformBufferInfo = {};
    uniformBufferInfo.buffer                 = m_mainBuffer;
    uniformBufferInfo.offset                 = ubInfo.m_offset;
    uniformBufferInfo.range                  = ubInfo.m_byteSize;

    VkWriteDescriptorSet uniformDescriptorWrite = {};
    uniformDescriptorWrite.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    uniformDescriptorWrite.dstSet               = m_descriptorSet;
    uniformDescriptorWrite.dstBinding           = 0;
    uniformDescriptorWrite.dstArrayElement      = 0;
    uniformDescriptorWrite.descriptorType       = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformDescriptorWrite.descriptorCount      = 1;
    uniformDescriptorWrite.pBufferInfo          = &uniformBufferInfo;
    uniformDescriptorWrite.pImageInfo           = nullptr;
    uniformDescriptorWrite.pTexelBufferView     = nullptr;

    descriptorWrites.PushBack(std::move(uniformDescriptorWrite));
  }

  vkUpdateDescriptorSets(m_device, descriptorWrites.GetSize(), descriptorWrites.Data(), 0, nullptr);
}

const VkDescriptorSet& VkDrawable::GetDescriptorSet() const {
  return m_descriptorSet;
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
    m_buffer(device, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, createInfo.m_byteSize, memoryProperties,
             phyDeviceMemoryProperties, logger),
    m_stagingBuffer(device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, createInfo.m_byteSize, memoryProperties,
                    phyDeviceMemoryProperties, logger),
    m_device(device),
    m_renderPass(renderPass),
    m_viewport(viewport),
    m_descriptorSetLayout(),
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
    log_VulkanRenderSystem(std::move(logger)) {
  CreateDescriptorInfo(createInfo);
}


DrawableID VkDrawablePool::CreateDrawable(const DrawableCreateInfo& createInfo) {
  VkDrawable drawable = VkDrawable(m_device, m_buffer.Real(), m_descriptorSetLayout, m_descriptorPool, createInfo,
                                   m_numVertexSlots, m_numInstanceSlots, m_numUniformSlots, GetAllocator(), log_VulkanRenderSystem);
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
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 2048);

  const U32 uniformCount = m_numUniformSlots;
  Containers::Vector<VkDescriptorSetLayoutBinding> layoutBindings(uniformCount, allocatorTemporary);

  for (const auto& uniformBufferPair : createInfo.m_uniformBuffers) {
    const auto& slot = uniformBufferPair.first;
    const auto& bufferDesc = uniformBufferPair.second;
    const auto vkShaderStage = ToVkShaderStageFlagBits(bufferDesc.m_stage);
    VERIFY_OPT(log_VulkanRenderSystem, vkShaderStage, "Unknown Shader Stage");

    VkCore::CreateUniformBufferBinding(layoutBindings, slot, bufferDesc, vkShaderStage.value());
  }

  m_descriptorSetLayout = VkCore::CreateDescriptorSetLayout(m_device, layoutBindings, log_VulkanRenderSystem);

  // TODO(vasumahesh1):[DESCRIPTOR]: Add support for more than 1 set
  const Containers::Vector<VkDescriptorSetLayout> sets({m_descriptorSetLayout}, allocatorTemporary);
  m_pipelineLayout = VkCore::CreatePipelineLayout(m_device, sets, log_VulkanRenderSystem);


  // TODO(vasumahesh1):[DESCRIPTOR]: How to use Uniform Buffer Arrays?
  VkDescriptorPoolSize uniformPoolSize = {};
  uniformPoolSize.type                 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uniformPoolSize.descriptorCount      = m_numUniformSlots;

  // TODO(vasumahesh1):[TEXTURE]: Add support for Descriptor Pools
  Vector<VkDescriptorPoolSize> descriptorPool(1, allocatorTemporary);
  descriptorPool.PushBack(uniformPoolSize);

  // TODO(vasumahesh1):[DESCRIPTOR]: 1 Set per Drawable?
  VkDescriptorPoolCreateInfo poolInfo = {};
  poolInfo.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount              = descriptorPool.GetSize();
  poolInfo.pPoolSizes                 = descriptorPool.Data();
  poolInfo.maxSets                    = createInfo.m_numDrawables;

  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool),
    "Unable to create Descriptor Pool");
}

void VkDrawablePool::Submit() {
  m_pipelineFactory.SetInputAssemblyStage(PrimitiveTopology::TriangleList);
  m_pipelineFactory.SetRasterizerStage(CullMode::None, FrontFace::CounterClockwise);
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

  VkBuffer mainBuffer = m_buffer.Real();

  for (auto& drawable : m_drawables) {
    drawable.Submit();

    const auto& vertBufferInfos = drawable.GetVertexBufferInfos();

    for (const auto& vertexBuffer : vertBufferInfos) {
      VkDeviceSize offsets[] = {vertexBuffer.m_offset};
      vkCmdBindVertexBuffers(m_commandBuffer, vertexBuffer.m_slot.m_binding, 1, &mainBuffer, &offsets[0]);
    }

    const auto& instanceBufferInfos = drawable.GetInstanceBufferInfos();

    for (const auto& instanceBuffer : instanceBufferInfos) {
      VkDeviceSize offsets[] = {instanceBuffer.m_offset};
      vkCmdBindVertexBuffers(m_commandBuffer, instanceBuffer.m_slot.m_binding, 1, &mainBuffer, &offsets[0]);
    }

    const auto& indexBufferInfo = drawable.GetIndexBufferInfo();

    const auto indexType = ToVkIndexType(drawable.GetIndexType());
    VERIFY_OPT(log_VulkanRenderSystem, indexType, "Invalid VkIndexType converted");

    vkCmdBindIndexBuffer(m_commandBuffer, mainBuffer, indexBufferInfo.m_offset, indexType.value());

    vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1,
                            &drawable.GetDescriptorSet(), 0, nullptr);

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

void VkDrawablePool::AddBufferBinding(Slot slot, const Containers::Vector<RawStorageFormat>& strides) {
  m_pipelineFactory.BulkAddAttributeDescription(strides, slot.m_binding);

  U32 totalBufferStride = 0;

  for (const auto& stride : strides) {
    totalBufferStride += GetFormatSize(stride);
  }

  m_pipelineFactory.AddBindingDescription(totalBufferStride, slot);
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
  vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayout, nullptr);

  vkFreeCommandBuffers(m_device, m_graphicsCommandPool, 1, &m_commandBuffer);

  for (const auto& shader : m_shaders) {
    shader.CleanUp(m_device);
  }
}

const VkCommandBuffer& VkDrawablePool::GetCommandBuffer() const {
  return m_commandBuffer;
}

void VkDrawablePool::BindVertexData(DrawableID drawableId, const Slot& slot, const U8* buffer, U32 size) {
  assert(m_drawables.GetSize() > drawableId);

  auto& drawable = m_drawables[drawableId];

  BufferInfo info    = BufferInfo();
  info.m_maxByteSize = size;
  info.m_byteSize    = size;
  info.m_offset      = m_mainBufferOffset;
  info.m_slot        = slot;

  drawable.AddVertexBufferInfo(std::move(info));
  AppendToMainBuffer(buffer, size);
}

void VkDrawablePool::BindInstanceData(DrawableID drawableId, const Slot& slot, const U8* buffer, U32 size) {
  assert(m_drawables.GetSize() > drawableId);

  auto& drawable = m_drawables[drawableId];

  BufferInfo info    = BufferInfo();
  info.m_maxByteSize = size;
  info.m_byteSize    = size;
  info.m_offset      = m_mainBufferOffset;
  info.m_slot        = slot;

  drawable.AddInstanceBufferInfo(std::move(info));
  AppendToMainBuffer(buffer, size);
}

void VkDrawablePool::BindUniformData(DrawableID drawableId, const Slot& slot, const U8* buffer, U32 size) {
  const auto minAlignment = U32(m_physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);

  assert(m_drawables.GetSize() > drawableId);

  if (minAlignment > 0) {
    size = (size + minAlignment - 1) & ~(minAlignment - 1);
  }

  auto& drawable = m_drawables[drawableId];

  const U32 currentOffset = m_mainBufferOffset;
  const U32 newOffset     = (currentOffset + minAlignment - 1) & ~(minAlignment - 1);
  m_mainBufferOffset += newOffset - currentOffset;

  BufferInfo info = BufferInfo();
  info.m_byteSize = size;
  info.m_offset   = newOffset;
  info.m_slot     = slot;

  drawable.AddUniformBufferInfo(std::move(info));
  AppendToMainBuffer(buffer, size);
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
