
.. _program_listing_file_Source_Azura_RenderSystem_Inc_Vulkan_VkShader.h:

Program Listing for File VkShader.h
===================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Inc_Vulkan_VkShader.h>` (``Source\Azura\RenderSystem\Inc\Vulkan\VkShader.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <vulkan/vulkan_core.h>
   #include <map>
   #include "Generic/Shader.h"
   #include "Log/Log.h"
   
   namespace Azura {
   enum class RawStorageFormat;
   }  // namespace Azura
   
   namespace Azura {
   namespace Vulkan {
   
   class VkShader : public Shader {
    public:
     VkShader(VkDevice device, const String& fileName, const Log& logger);
     VkPipelineShaderStageCreateInfo GetShaderStageInfo() const;
   
     void CleanUp(VkDevice device) const;
   
    private:
     VkShaderModule m_module{};
     const Log log_VulkanRenderSystem;
   };
   
   }  // namespace Vulkan
   }  // namespace Azura
