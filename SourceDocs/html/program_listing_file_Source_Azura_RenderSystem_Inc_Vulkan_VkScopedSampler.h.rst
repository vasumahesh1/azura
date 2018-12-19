
.. _program_listing_file_Source_Azura_RenderSystem_Inc_Vulkan_VkScopedSampler.h:

Program Listing for File VkScopedSampler.h
==========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Inc_Vulkan_VkScopedSampler.h>` (``Source\Azura\RenderSystem\Inc\Vulkan\VkScopedSampler.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <vulkan/vulkan_core.h>
   #include "Log/Log.h"
   
   namespace Azura {
   namespace Vulkan {
   
   class VkScopedSampler {
   public:
     VkScopedSampler(VkDevice device, Log logger);
   
     VkSampler Real() const;
   
     void CleanUp() const;
   
   private:
     VkDevice m_device;
   
     VkSampler m_sampler{};
   
     const Log log_VulkanRenderSystem;
   };
   
   } // namespace Vulkan
   } // namespace Azura
