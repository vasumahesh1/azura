#pragma once

#include <vulkan/vulkan_core.h>

#include "Generic/Drawable.h"
#include "VkScopedBuffer.h"
#include "VkShader.h"
#include "VkScopedPipeline.h"
#include "Log/Log.h"
#include "VkScopedImage.h"
#include "VkScopedSampler.h"

namespace Azura {
struct ApplicationRequirements;
}

namespace Azura {
namespace Vulkan {
struct VkScopedSwapChain;


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
                 VkRenderPass renderPass,
                 const ApplicationRequirements& appReq,
                 const ViewportDimensions& viewport,
                 const VkPhysicalDeviceMemoryProperties& phyDeviceMemoryProperties,
                 const VkPhysicalDeviceProperties& physicalDeviceProperties,
                 const VkScopedSwapChain& swapChain,
                 Memory::Allocator& allocator,
                 Memory::Allocator& allocatorTemporary,
                 Log logger);

  DrawableID CreateDrawable(const DrawableCreateInfo& createInfo) override;

  void CreateDescriptorInfo(const DrawablePoolCreateInfo& createInfo);

  void Submit() override;
  void AddBufferBinding(SlotID slotId, const Containers::Vector<RawStorageFormat>& strides) override;

  void AddShader(const Shader& shader) override;

  void CleanUp() const;

  const VkCommandBuffer& GetCommandBuffer() const;

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
  VkRenderPass m_renderPass;
  ViewportDimensions m_viewport;
  VkPhysicalDeviceMemoryProperties m_physicalDeviceMemoryProperties;

  VkDescriptorPool m_descriptorPool{};
  Containers::Vector<VkDescriptorSetLayout> m_descriptorSetLayouts;

  VkScopedPipeline m_pipeline;
  VkPipelineLayout m_pipelineLayout;
  VkPipelineFactory m_pipelineFactory;

  VkCommandBuffer m_commandBuffer{};
  VkCommandPool m_graphicsCommandPool;

  VkQueue m_graphicsQueue;

  U32 m_mainBufferOffset{0};

  const VkScopedSwapChain& m_swapChain;
  const ApplicationRequirements& m_appRequirements;
  const VkPhysicalDeviceProperties& m_physicalDeviceProperties;

  Containers::Vector<VkDrawable> m_drawables;
  Containers::Vector<VkShader> m_shaders;
  Containers::Vector<VkScopedImage> m_images;
  Containers::Vector<VkScopedSampler> m_samplers;

  const Log log_VulkanRenderSystem;
};

} // namespace Vulkan
} // namespace Azura
