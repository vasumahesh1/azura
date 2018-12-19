
.. _program_listing_file_Source_Azura_RenderSystem_Inc_Vulkan_VkTextureManager.h:

Program Listing for File VkTextureManager.h
===========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Inc_Vulkan_VkTextureManager.h>` (``Source\Azura\RenderSystem\Inc\Vulkan\VkTextureManager.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "Generic/TextureManager.h"
   #include <vulkan/vulkan_core.h>
   #include "Generic/Renderer.h"
   
   namespace Azura {
   namespace Vulkan {
   
   class VkTextureManager final : public TextureManager
   {
   public:
     VkTextureManager(const TextureRequirements& requirements);
   
   private:
   };
   
   } // namespace Vulkan
   } // namespace Azura
