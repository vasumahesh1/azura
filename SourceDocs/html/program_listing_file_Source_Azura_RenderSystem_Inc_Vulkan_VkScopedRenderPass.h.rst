
.. _program_listing_file_Source_Azura_RenderSystem_Inc_Vulkan_VkScopedRenderPass.h:

Program Listing for File VkScopedRenderPass.h
=============================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Inc_Vulkan_VkScopedRenderPass.h>` (``Source\Azura\RenderSystem\Inc\Vulkan\VkScopedRenderPass.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include <vulkan/vulkan_core.h>
   #include "Log/Log.h"
   #include "Generic/GenericTypes.h"
   #include "VkScopedImage.h"
   #include "VkShader.h"
   
   namespace Azura {
   namespace Vulkan {
   class VkScopedSwapChain;
   
   class VkScopedRenderPass {
   public:
     VkScopedRenderPass(U32 idx, Memory::Allocator& mainAllocator, Log logger);
   
     void Create(VkDevice device,
                 VkCommandPool commandPool,
                 const PipelinePassCreateInfo& createInfo,
                 const Containers::Vector<RenderTargetCreateInfo>& pipelineBuffers,
                 const Containers::Vector<VkScopedImage>& pipelineBufferImages,
                 const Containers::Vector<VkShader>& allShaders,
                 const VkScopedSwapChain& swapChain);
   
     void CreateForSwapChain(VkDevice device,
       VkCommandPool commandPool,
       const PipelinePassCreateInfo& createInfo,
       const Containers::Vector<VkShader>& allShaders,
       const VkScopedSwapChain& swapChain);
   
     VkRenderPass GetRenderPass() const;
     VkFramebuffer GetFrameBuffer(U32 idx) const;
     VkCommandBuffer GetCommandBuffer(U32 idx) const;
   
     const Containers::Vector<PipelinePassInput>& GetPassInputs() const;
   
     U32 GetFrameBufferCount() const;
     VkSemaphore GetRenderSemaphore() const;
     VkDescriptorSetLayout GetDescriptorSetLayout() const;
   
     U32 GetId() const;
   
     U32 GetDescriptorSetId() const;
     void SetDescriptorSetId(U32 id);
   
     void Begin(const VkScopedSwapChain& swapChain) const;
     void End() const;
   
     const Containers::Vector<VkPipelineShaderStageCreateInfo>& GetShaderStageInfo() const;
     const Containers::Vector<VkPipelineColorBlendAttachmentState>& GetColorBlendAttachments() const;
   
   
     void CleanUp(VkDevice device, VkCommandPool commandPool) const;
   
   private:
     void CreateDescriptorSetLayout(VkDevice device, const PipelinePassCreateInfo& createInfo);
   
     Log log_VulkanRenderSystem;
   
     U32 m_id;
     U32 m_descriptorSet{0};
     U32 m_numAttachments{0};
   
     VkSemaphore m_beginRenderSemaphore{};
     VkDescriptorSetLayout m_descriptorSetLayout;
   
     Containers::Vector<VkFramebuffer> m_frameBuffers;
     Containers::Vector<VkCommandBuffer> m_commandBuffers;
     Containers::Vector<PipelinePassInput> m_inputAttachments;
     Containers::Vector<VkPipelineColorBlendAttachmentState> m_colorBlendAttachments;
     Containers::Vector<VkClearValue> m_clearValues;
   
     VkRenderPass m_renderPass{};
     Containers::Vector<VkPipelineShaderStageCreateInfo> m_shaderPipelineInfos;
   };
   
   } // namespace Vulkan
   } // namespace Azura
