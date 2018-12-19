
.. _program_listing_file_Source_Azura_RenderSystem_Src_Vulkan_VkTextureManager.cpp:

Program Listing for File VkTextureManager.cpp
=============================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Src_Vulkan_VkTextureManager.cpp>` (``Source\Azura\RenderSystem\Src\Vulkan\VkTextureManager.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include <utility>
   #include "Vulkan/VkTextureManager.h"
   
   namespace Azura {
   namespace Vulkan {
   
   VkTextureManager::VkTextureManager(const TextureRequirements& requirements)
     : TextureManager(requirements) {
   }
   } // namespace Vulkan
   } // namespace Azura
