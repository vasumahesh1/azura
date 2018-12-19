
.. _program_listing_file_Source_Azura_RenderSystem_Src_Vulkan_Windows_VkWin32RenderSystem.cpp:

Program Listing for File VkWin32RenderSystem.cpp
================================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Src_Vulkan_Windows_VkWin32RenderSystem.cpp>` (``Source\Azura\RenderSystem\Src\Vulkan\Windows\VkWin32RenderSystem.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Generic/RenderSystem.h"
   #include "Generic/Windows/Win32GLFWWindow.h"
   
   namespace Azura {
   std::unique_ptr<Window> RenderSystem::CreateApplicationWindow(String title, U32 width, U32 height) {
     return std::make_unique<Win32GLFWWindow>(title, width, height);
   }
   }  // namespace Azura
