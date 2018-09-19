#pragma once

#include <vulkan/vulkan_core.h>

#include "Generic/Drawable.h"
#include "VkScopedBuffer.h"
#include "VkShader.h"
#include "VkScopedPipeline.h"
#include "Log/Log.h"

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
             VkDescriptorSetLayout descriptorSetLayout,
             VkDescriptorPool descriptorPool,
             const DrawableCreateInfo& info,
    U32 numVertexSlots,
    U32 numInstanceSlots,
    U32 numUniformSlots,
             Memory::Allocator& allocator,
             Log logger);

  void Submit() override;

  const VkDescriptorSet& GetDescriptorSet() const;

  void CleanUp(VkDevice device) const;

private:
  VkDevice m_device;
  VkDescriptorSetLayout m_descriptorSetLayout;
  VkDescriptorPool m_descriptorPool;
  VkBuffer m_mainBuffer;

  VkDescriptorSet m_descriptorSet;
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
  void AddBufferBinding(Slot slot, const Containers::Vector<RawStorageFormat>& strides) override;

  void AddShader(const Shader& shader) override;

  void CleanUp() const;

  const VkCommandBuffer& GetCommandBuffer() const;

  void BindVertexData(DrawableID drawableId, const Slot& slot, const U8* buffer, U32 size) override;
  void BindInstanceData(DrawableID drawableId, const Slot& slot, const U8* buffer, U32 size) override;
  void BindUniformData(DrawableID drawableId, const Slot& slot, const U8* buffer, U32 size) override;
  void SetIndexData(DrawableID drawableId, const U8* buffer, U32 size) override;

  void AppendToMainBuffer(const U8* buffer, U32 bufferSize);

private:
  VkScopedBuffer m_buffer;
  VkScopedBuffer m_stagingBuffer;
  VkDevice m_device;
  VkRenderPass m_renderPass;
  ViewportDimensions m_viewport;

  VkDescriptorPool m_descriptorPool{};
  VkDescriptorSetLayout m_descriptorSetLayout;

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

  const Log log_VulkanRenderSystem;
};

} // namespace Vulkan
} // namespace Azura
