#pragma once

#include <vulkan/vulkan_core.h>

#include "Generic/Drawable.h"
#include "VkScopedBuffer.h"
#include "VkScopedPipeline.h"
#include "Log/Log.h"
#include "VkScopedImage.h"
#include "VkScopedSampler.h"
#include "VkScopedRenderPass.h"

namespace Azura {
struct ApplicationRequirements;
}

namespace Azura {
namespace Vulkan {

class VkDrawablePool;

class VkDrawable : public Drawable {
public:
  VkDrawable(VkDevice device,
             VkBuffer mainBuffer,
             const Containers::Vector<VkDescriptorSetLayout>& descriptorSetLayouts,
             VkDescriptorPool descriptorPool,
             const DrawableCreateInfo& info,
             U32 numVertexSlots,
             U32 numInstanceSlots,
             U32 numUniformSlots,
             Memory::Allocator& allocator,
             Log logger);

  void WriteDescriptorSets(
    const Containers::Vector<TextureBufferInfo>& textureBufferInfos,
    const Containers::Vector<SamplerInfo>& samplerInfos,
    const Containers::Vector<VkScopedSampler>& samplers,
    const Containers::Vector<VkScopedImage>& images);

  const Containers::Vector<VkDescriptorSet>& GetDescriptorSet() const;

  void CleanUp(VkDevice device) const;

private:
  VkDevice m_device;
  Containers::Vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
  VkDescriptorPool m_descriptorPool;
  VkBuffer m_mainBuffer;

  Containers::Vector<VkDescriptorSet> m_descriptorSets;
  const Log log_VulkanRenderSystem;
};

class VkDrawablePool final : public DrawablePool {
  friend class VkDrawable;

public:
  VkDrawablePool(const DrawablePoolCreateInfo& createInfo,
                 VkDevice device,
                 VkQueue graphicsQueue,
                 VkBufferUsageFlags usage,
                 VkMemoryPropertyFlags memoryProperties,
                 VkCommandPool graphicsCommandPool,
                 VkPipelineLayout pipelineLayout,
                 VkDescriptorPool descriptorPool,
                 const Containers::Vector<VkDescriptorSetLayout>& descriptorSetLayouts,
                 const Containers::Vector<VkScopedRenderPass>& renderPasses,
                 const ApplicationRequirements& appReq,
                 const ViewportDimensions& viewport,
                 const VkPhysicalDeviceMemoryProperties& phyDeviceMemoryProperties,
                 const VkPhysicalDeviceProperties& physicalDeviceProperties,
                 const VkScopedSwapChain& swapChain,
                 const Containers::Vector<DescriptorSlot>& descriptorSlots,
                 const DescriptorCount& descriptorCount,
                 Memory::Allocator& allocator,
                 Memory::Allocator& allocatorTemporary,
                 Log logger);

  DrawableID CreateDrawable(const DrawableCreateInfo& createInfo) override;

  void Submit() override;
  void AddBufferBinding(SlotID slotId, const Containers::Vector<RawStorageFormat>& strides) override;

  void CleanUp() const;

  const VkCommandBuffer& GetCommandBuffer(U32 idx) const;

  // Drawable Scope Binds
  void BindVertexData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) override;
  void BindInstanceData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) override;
  void BindUniformData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) override;

  // Pool Scope Binds
  void BindTextureData(SlotID slot, const TextureDesc& desc, const U8* buffer) override;
  void BindSampler(SlotID slot, const SamplerDesc& desc) override;

  void SetIndexData(DrawableID drawableId, const U8* buffer, U32 size) override;

  void AppendToMainBuffer(const U8* buffer, U32 bufferSize);

private:
  void SubmitTextureData();

  VkScopedBuffer m_buffer;
  VkScopedBuffer m_stagingBuffer;
  VkDevice m_device;
  const Containers::Vector<VkScopedRenderPass>& m_renderPasses;
  const Containers::Vector<VkDescriptorSetLayout>& m_descriptorSetLayouts;
  const Containers::Vector<DescriptorSlot>& m_descriptorSlots;

  ViewportDimensions m_viewport;
  VkPhysicalDeviceMemoryProperties m_physicalDeviceMemoryProperties;

  VkDescriptorPool m_descriptorPool;

  Containers::Vector<VkScopedPipeline> m_pipelines;
  VkPipelineLayout m_pipelineLayout;
  VkPipelineFactory m_pipelineFactory;

  Containers::Vector<VkCommandBuffer> m_commandBuffers;
  VkCommandPool m_graphicsCommandPool;

  VkQueue m_graphicsQueue;

  U32 m_mainBufferOffset{0};

  const VkScopedSwapChain& m_swapChain;
  const ApplicationRequirements& m_appRequirements;
  const VkPhysicalDeviceProperties& m_physicalDeviceProperties;

  Containers::Vector<VkDrawable> m_drawables;
  Containers::Vector<VkScopedImage> m_images;
  Containers::Vector<VkScopedSampler> m_samplers;

  const Log log_VulkanRenderSystem;
};

} // namespace Vulkan
} // namespace Azura
