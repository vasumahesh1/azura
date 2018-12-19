
.. _program_listing_file_Source_Azura_RenderSystem_Inc_Vulkan_VkPlatform.h:

Program Listing for File VkPlatform.h
=====================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Inc_Vulkan_VkPlatform.h>` (``Source\Azura\RenderSystem\Inc\Vulkan\VkPlatform.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <vulkan/vulkan_core.h>
   #include "Containers/Vector.h"
   #include "Log/Log.h"
   
   namespace Azura {
   namespace Vulkan {
   
   namespace VkPlatform {
   VkSurfaceKHR CreateSurface(const void* windowHandle, VkInstance instance, const Log& log_VulkanRenderSystem);
   void GetInstanceExtensions(Containers::Vector<const char*>& extensions);
   }  // namespace VkPlatform
   
   }  // namespace Vulkan
   }  // namespace Azura
