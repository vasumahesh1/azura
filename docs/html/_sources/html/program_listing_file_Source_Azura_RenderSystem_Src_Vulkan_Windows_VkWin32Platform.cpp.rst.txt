
.. _program_listing_file_Source_Azura_RenderSystem_Src_Vulkan_Windows_VkWin32Platform.cpp:

Program Listing for File VkWin32Platform.cpp
============================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Src_Vulkan_Windows_VkWin32Platform.cpp>` (``Source\Azura\RenderSystem\Src\Vulkan\Windows\VkWin32Platform.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Vulkan/VkPlatform.h"
   
   #include <Windows.h>
   
   #include <vulkan/vulkan_win32.h>
   #include "Vulkan/VkMacros.h"
   
   namespace Azura {
   namespace Vulkan {
   
   VkSurfaceKHR VkPlatform::CreateSurface(const void* windowHandle, VkInstance instance, const Log& log_VulkanRenderSystem) {
     VkWin32SurfaceCreateInfoKHR createInfo = {};
     createInfo.sType                       = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
   
     // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
     createInfo.hwnd      = *reinterpret_cast<const HWND*>(windowHandle);
     createInfo.hinstance = GetModuleHandle(nullptr);
   
     const auto CreateWin32SurfaceKHR =
         // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
         reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR"));
     FAIL_IF(log_VulkanRenderSystem, CreateWin32SurfaceKHR == nullptr, "Cannot find PFN_vkCreateWin32SurfaceKHR");
   
     VkSurfaceKHR surface;
     VERIFY_VK_OP(log_VulkanRenderSystem, CreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface), "Failed to create window surface");
     return surface;
   }
   
   void VkPlatform::GetInstanceExtensions(Containers::Vector<const char*>& extensions) {
     extensions.PushBack(VK_KHR_SURFACE);
     extensions.PushBack("VK_KHR_win32_surface");
   
   #ifdef BUILD_DEBUG
     extensions.PushBack(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
   #endif
   }
   }  // namespace Vulkan
   }  // namespace Azura
