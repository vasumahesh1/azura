
.. _program_listing_file_Source_Azura_RenderSystem_Inc_Vulkan_VkDebug.h:

Program Listing for File VkDebug.h
==================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Inc_Vulkan_VkDebug.h>` (``Source\Azura\RenderSystem\Inc\Vulkan\VkDebug.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include <vulkan/vulkan_core.h>
   
   namespace Azura {
   namespace Vulkan {
   
   const char* VkResultToString(const VkResult err);
   const char* VkPhysicalDeviceTypeToString(const VkPhysicalDeviceType deviceType);
   } // namespace Vulkan
   } // namespace Azura
