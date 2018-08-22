#include "Vulkan/VkWindow.h"

#include <Windows.h>

#include <vulkan/vulkan_win32.h>
#include "Vulkan/VkMacros.h"

namespace Azura {
namespace Vulkan {

VkWindow::VkWindow(void* windowResource, const U32 width, const U32 height)
  : Window(windowResource, width, height) {
}

VkSurfaceKHR VkWindow::CreateSurface(VkInstance instance) const {
  VkWin32SurfaceCreateInfoKHR createInfo = {};
  createInfo.sType                       = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  createInfo.hwnd      = *reinterpret_cast<const HWND*>(Handle());
  createInfo.hinstance = GetModuleHandle(nullptr);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto CreateWin32SurfaceKHR = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(
    vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR"));
  VERIFY_OPT(!CreateWin32SurfaceKHR, "Cannot find PFN_vkCreateWin32SurfaceKHR");

  VkSurfaceKHR surface;
  VERIFY_VK_OP(CreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface), "Failed to create window surface");
  return surface;
}

void VkWindow::GetInstanceExtensions(Containers::Vector<const char*>& extensions) {
  extensions.PushBack(VK_KHR_SURFACE);
  extensions.PushBack("VK_KHR_win32_surface");
}
} // namespace Vulkan
} // namespace Azura
