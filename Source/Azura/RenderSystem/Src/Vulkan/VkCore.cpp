#include "Vulkan/VkCore.h"
#include "Generic/Constants.h"

#include <array>
#include <map>
#include <set>
#include "Utils/Macros.h"
#include "Vulkan/VkTypeMapping.h"

namespace Azura {
namespace Vulkan {
namespace {
#define VERIFY_VK_OP(res, message)        \
  if((res) != VK_SUCCESS) {               \
    throw std::runtime_error(#message);   \
  }

#define VERIFY_TRUE(res, message)         \
  if((res) != true) {                     \
    throw std::runtime_error(#message);   \
  }

#define VERIFY_OPT(res, message)         \
  if((res)) {                     \
    throw std::runtime_error(#message);   \
  }

#ifdef DEBUG
    const std::array<const char*, 1> VALIDATION_LAYERS = {
      "VK_LAYER_LUNARG_standard_validation"
    };
#endif

const std::array<const char*, 1> DEVICE_EXTENSIONS = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

bool CheckValidationLayerSupport() {
#ifdef DEBUG
  uint32_t layerCount = 0;

  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  std::vector<VkLayerProperties> availableLayers(layerCount);

  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char* layer : VALIDATION_LAYERS) {
    bool layerFound = false;
    for (const auto& vkLayer : availableLayers) {
      if (strcmp(layer, vkLayer.layerName) == 0) {
        layerFound = true;
        break;
      }
    }

    if (!layerFound) {
      return false;
    }
  }
#endif

  return true;
}

bool CheckDeviceExtensionSupport(VkPhysicalDevice device) {
  uint32_t extensionCount = 0;

  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

  for (const auto& requiredExtension : DEVICE_EXTENSIONS) {
    bool foundExtension = false;
    for (const auto& availableExtension : availableExtensions) {
      if (strcmp(&availableExtension.extensionName[0], requiredExtension) == 0) {
        foundExtension = true;
        break;
      }
    }

    if (!foundExtension) {
      return false;
    }
  }

  return true;
}

int GetDeviceScore(VkPhysicalDevice device,
                   VkSurfaceKHR surface,
                   const DeviceRequirements& requirements,
                   const SwapChainDeviceSupport& swapChainSupport) {
  int score = 0;
  VkPhysicalDeviceProperties deviceProperties;
  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceProperties(device, &deviceProperties);
  vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

  const VkQueueIndices indices      = VkCore::FindQueueFamiliesInDevice(device, surface, requirements);
  const bool areExtensionsSupported = CheckDeviceExtensionSupport(device);

  if (!areExtensionsSupported) {
    return 0;
  }

  const bool isSwapChainSupported = swapChainSupport.IsSupported();

  if (!isSwapChainSupported) {
    return 0;
  }

  if (!indices.IsComplete()) {
    return 0;
  }

  if (requirements.m_float64 && deviceFeatures.shaderFloat64 == 0u) {
    return 0;
  }

  if (requirements.m_int64 && deviceFeatures.shaderInt64 == 0u) {
    return 0;
  }

  if (requirements.m_discreteGPU && deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    score += 1000;
  }

  return score;
}

VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats,
                                           const SwapChainRequirement& requirement) {
  const auto format = ToVkFormat(requirement.m_format);
  VERIFY_OPT(format, "Unknown Format");

  const auto colorSpace = ToVkColorSpaceKHR(requirement.m_colorSpace);
  VERIFY_OPT(colorSpace, "Unknown Colorspace");

  // no preferred format
  if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
    return {format.value(), colorSpace.value()};
  }

  for (const auto& availableFormat : availableFormats) {
    if (availableFormat.format == format.value() && availableFormat.colorSpace == colorSpace.value()) {
      return availableFormat;
    }
  }

  return availableFormats[0];
}

VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
  VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;
  for (const auto& availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availablePresentMode;
    }

    if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
      bestMode = availablePresentMode;
    }
  }

  return bestMode;
}

VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const SwapChainRequirement& requirement) {
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  }

  VkExtent2D actualExtent = {requirement.m_extent.m_width, requirement.m_extent.m_height};
  actualExtent.width      = std::max(capabilities.minImageExtent.width,
                                     std::min(capabilities.maxImageExtent.width, actualExtent.width));
  actualExtent.height = std::max(capabilities.minImageExtent.height,
                                 std::min(capabilities.maxImageExtent.height, actualExtent.height));
  return actualExtent;
}
} // namespace

VkInstance VkCore::CreateInstance(const ApplicationInfo& applicationData, std::vector<const char*> vkExtensions) {
  VkApplicationInfo appInfo = {};
  appInfo.sType             = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName  = applicationData.m_name.c_str();

  const std::uint32_t majorVer = std::get<SemverMajor>(applicationData.m_version);
  const std::uint32_t minorVer = std::get<SemverMinor>(applicationData.m_version);
  const std::uint32_t patchVer = std::get<SemverPatch>(applicationData.m_version);
  appInfo.applicationVersion   = VK_MAKE_VERSION(majorVer, minorVer, patchVer);

  appInfo.pEngineName   = "";
  appInfo.engineVersion = VK_MAKE_VERSION(RENDER_SYSTEM_MAJOR_SEMVER, RENDER_SYSTEM_MINOR_SEMVER,
    RENDER_SYSTEM_PATCH_SEMVER);
  appInfo.apiVersion = VK_API_VERSION_1_1;

  VkInstanceCreateInfo createInfo    = {};
  createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo        = &appInfo;
  createInfo.enabledExtensionCount   = uint32_t(vkExtensions.size());
  createInfo.ppEnabledExtensionNames = vkExtensions.data();

  // Validation Layers Check
  VERIFY_TRUE(CheckValidationLayerSupport(), "Validation layers requested, but not available on device");

#ifdef DEBUG
  createInfo.enabledLayerCount   = uint32_t(VALIDATION_LAYERS.size());
  createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
#else
  createInfo.enabledLayerCount   = 0;
  createInfo.ppEnabledLayerNames = nullptr;
#endif

  VkInstance result;
  VERIFY_VK_OP(vkCreateInstance(&createInfo, nullptr, &result), "Failed to create VkInstance");
  return result;
}

VkQueueIndices VkCore::FindQueueFamiliesInDevice(VkPhysicalDevice device,
                                                 VkSurfaceKHR surface,
                                                 const DeviceRequirements& requirements) {
  VkQueueIndices result;
  result.m_isTransferQueueRequired = requirements.m_transferQueue;

  uint32_t queueFamilyCount;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

  int idx = 0;
  for (const auto& queueFamily : queueFamilies) {
    if (queueFamily.queueCount > 0 && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
      result.m_graphicsFamily = idx;
    }

    if (queueFamily.queueCount > 0 && !((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) && (
          queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) {
      result.m_transferFamily = idx;
    }

    VkBool32 canPresent = 0u;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, idx, surface, &canPresent);

    if (queueFamily.queueCount > 0 && canPresent != 0u) {
      result.m_presentFamily = idx;
    }

    ++idx;
  }

  return result;
}

SwapChainDeviceSupport VkCore::QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
  SwapChainDeviceSupport result;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &result.m_capabilities);
  uint32_t formatCount = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

  if (formatCount > 0) {
    result.m_formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, result.m_formats.data());
  }

  uint32_t presentCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentCount, nullptr);

  if (presentCount > 0) {
    result.m_presentModes.resize(presentCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentCount, result.m_presentModes.data());
  }

  return result;
}

VkPhysicalDevice VkCore::SelectPhysicalDevice(VkInstance instance,
                                              VkSurfaceKHR surface,
                                              const DeviceRequirements& requirements,
                                              const SwapChainDeviceSupport& swapChainSupport) {
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

  // Query for Available Devices
  uint32_t availableDeviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &availableDeviceCount, nullptr);

  VERIFY_TRUE(availableDeviceCount != 0, "No supported GPUs found with Vulkan");

  std::vector<VkPhysicalDevice> availableDevices(availableDeviceCount);
  vkEnumeratePhysicalDevices(instance, &availableDeviceCount, availableDevices.data());

  std::multimap<int, VkPhysicalDevice> candidates;

  // Rate and Select the best GPU
  for (const auto& device : availableDevices) {
    int tempScore = GetDeviceScore(device, surface, requirements, swapChainSupport);
    candidates.insert(std::make_pair(tempScore, device));
  }

  if (candidates.rbegin()->first > 0) {
    physicalDevice = candidates.rbegin()->second;
  }

  VERIFY_TRUE(physicalDevice != VK_NULL_HANDLE, "No suitable GPU found for Vulkan");
  return physicalDevice;
}

VkDevice VkCore::CreateLogicalDevice(VkPhysicalDevice physicalDevice,
                                     const VkQueueIndices& queueIndices,
                                     const DeviceRequirements& requirements) {
  // Create Queue Infos
  const std::set<int> uniqueQueueFamilies = {
    queueIndices.m_graphicsFamily,
    queueIndices.m_presentFamily,
    queueIndices.m_transferFamily
  };

  // TODO(vasumahesh1): Simplify with Azura::Vector
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

  float queuePriority = 1.0f;

  for (const auto& queueIdx : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex        = queueIdx;
    queueCreateInfo.queueCount              = 1;
    queueCreateInfo.pQueuePriorities        = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures = {};
  deviceFeatures.shaderFloat64            = requirements.m_float64 ? VK_TRUE : VK_FALSE;
  deviceFeatures.shaderInt64              = requirements.m_int64 ? VK_TRUE : VK_FALSE;

  // Create the logical device
  VkDeviceCreateInfo createInfo      = {};
  createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos       = queueCreateInfos.data();
  createInfo.queueCreateInfoCount    = uint32_t(queueCreateInfos.size());
  createInfo.pEnabledFeatures        = &deviceFeatures;
  createInfo.enabledExtensionCount   = uint32_t(DEVICE_EXTENSIONS.size());
  createInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

#ifdef DEBUG
  createInfo.enabledLayerCount   = uint32_t(VALIDATION_LAYERS.size());
  createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
#else
  createInfo.enabledLayerCount   = 0;
  createInfo.ppEnabledLayerNames = nullptr;
#endif

  VkDevice device;
  VERIFY_VK_OP(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device), "Failed to create VkDevice");
  return device;
}

VkQueue VkCore::GetQueueFromDevice(VkDevice device, int queueIndex) {
  VkQueue queue;
  vkGetDeviceQueue(device, queueIndex, 0, &queue);
  return queue;
}

SwapChainCreateResult VkCore::CreateSwapChain(VkDevice device,
                                              VkSurfaceKHR surface,
                                              const VkQueueIndices& queueIndices,
                                              const SwapChainDeviceSupport& swapChainSupport,
                                              const SwapChainRequirement& swapChainRequirement) {

  SwapChainCreateResult result;

  const VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.m_presentModes);
  result.m_surfaceFormat             = ChooseSwapSurfaceFormat(swapChainSupport.m_formats, swapChainRequirement);
  result.m_extent                    = ChooseSwapExtent(swapChainSupport.m_capabilities, swapChainRequirement);

  // Set Queue Length of SwapChain
  result.m_imageCount = swapChainSupport.m_capabilities.minImageCount + 1;
  if (swapChainSupport.m_capabilities.maxImageCount > 0 && result.m_imageCount > swapChainSupport
                                                                                 .m_capabilities.maxImageCount) {
    result.m_imageCount = swapChainSupport.m_capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo = {};
  createInfo.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface                  = surface;
  createInfo.minImageCount            = result.m_imageCount;
  createInfo.imageFormat              = result.m_surfaceFormat.format;
  createInfo.imageColorSpace          = result.m_surfaceFormat.colorSpace;
  createInfo.imageExtent              = result.m_extent;
  createInfo.imageArrayLayers         = 1;
  createInfo.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  // TODO(vasumahesh1): Possible bug if Graphics Queue is not the same as Present Queue. How to handle?
  if (queueIndices.m_transferFamily != -1) {
    createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = queueIndices.GetActiveSize();
    createInfo.pQueueFamilyIndices   = queueIndices.GetIndicesArray().data();
  } else {
    createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices   = nullptr;
  }

  createInfo.preTransform   = swapChainSupport.m_capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode    = presentMode;
  createInfo.clipped        = VK_TRUE;
  createInfo.oldSwapchain   = VK_NULL_HANDLE;

  VERIFY_VK_OP(vkCreateSwapchainKHR(device, &createInfo, nullptr, &result.m_swapChain), "Failed to create swap chain");

  return result;
}

std::vector<VkImage> VkCore::GetSwapChainImages(VkDevice device, VkSwapchainKHR swapChain, U32 imageCount) {
  std::vector<VkImage> result;

  vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
  result.resize(imageCount);
  vkGetSwapchainImagesKHR(device, swapChain, &imageCount, result.data());

  return result;
}

void VkCore::CreateRenderPass() {
}

void VkCore::CreateDescriptorSetLayout() {
}

void VkCore::CreateGraphicsPipelineLayout() {
}

void VkCore::CreateShaderModule(const std::vector<U8>& code) {
  UNUSED(code);
}

bool SwapChainDeviceSupport::IsSupported() const {
  return !m_formats.empty() && !m_presentModes.empty();
}

} // namespace Vulkan
} // namespace Azura
