#include "Vulkan/VulkanCore.h"
#include "Generic/Constants.h"

#include <array>
#include <map>
#include <set>

namespace Azura {

namespace {
#define VERIFY_VK_OP(res, message)        \
  if((res) != VK_SUCCESS) {               \
    throw std::runtime_error(#message);   \
  }

#define VERIFY_TRUE(res, message)         \
  if((res) != true) {                     \
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
      if (strcmp(availableExtension.extensionName, requiredExtension) == 0) {
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

VkQueueIndices FindQueueFamiliesInDevice(VkPhysicalDevice device, VkSurfaceKHR surface) {
  VkQueueIndices result;

  uint32_t queueFamilyCount;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

  int idx = 0;
  for (const auto& queueFamily : queueFamilies) {
    if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      result.m_graphicsFamily = idx;
    }

    if (queueFamily.queueCount > 0 && !(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) && queueFamily.queueFlags &&
        VK_QUEUE_TRANSFER_BIT) {
      result.m_transferFamily = idx;
    }

    VkBool32 canPresent = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, idx, surface, &canPresent);

    if (queueFamily.queueCount > 0 && canPresent) {
      result.m_presentFamily = idx;
    }

    // Force get everything unless loop finishes
    if (result.IsComplete(true)) {
      break;
    }

    ++idx;
  }

  return result;
}

SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
  SwapChainSupportDetails result;

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

int GetDeviceScore(VkPhysicalDevice device, VkSurfaceKHR surface, const DeviceRequirements& requirements) {
  int score = 0;
  VkPhysicalDeviceProperties deviceProperties;
  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceProperties(device, &deviceProperties);
  vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

  const VkQueueIndices indices      = FindQueueFamiliesInDevice(device, surface);
  const bool areExtensionsSupported = CheckDeviceExtensionSupport(device);

  if (!areExtensionsSupported) {
    return 0;
  }

  const bool isSwapChainSupported = [&]()
  {
    bool isSupported = false;
    if (areExtensionsSupported) {
      const SwapChainSupportDetails details = QuerySwapChainSupport(device, surface);

      isSupported = !details.m_formats.empty() && !details.m_presentModes.empty();
    }

    return isSupported;
  }();

  if (!isSwapChainSupported) {
    return 0;
  }

  if (!indices.IsComplete(requirements.m_transferQueue)) {
    return 0;
  }

  if (requirements.m_float64 && !deviceFeatures.shaderFloat64) {
    return 0;
  }

  if (requirements.m_int64 && !deviceFeatures.shaderInt64) {
    return 0;
  }

  if (requirements.m_discreteGPU && deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    score += 1000;
  }

  return score;
}
} // namespace

VkInstance VulkanCore::CreateInstance(const ApplicationInfo& applicationData, Vector<const char*> vkExtensions) {
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
  createInfo.enabledExtensionCount   = uint32_t(vkExtensions.GetSize());
  createInfo.ppEnabledExtensionNames = vkExtensions.Data();

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

VkPhysicalDevice VulkanCore::SelectPhysicalDevice(VkInstance instance,
                                                  VkSurfaceKHR surface,
                                                  const DeviceRequirements& requirements) {
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
    int tempScore = GetDeviceScore(device, surface, requirements);
    candidates.insert(std::make_pair(tempScore, device));
  }

  if (candidates.rbegin()->first > 0) {
    physicalDevice = candidates.rbegin()->second;
  }

  VERIFY_TRUE(physicalDevice != VK_NULL_HANDLE, "No suitable GPU found for Vulkan");
  return physicalDevice;
}

VkDevice VulkanCore::CreateLogicalDevice(VkPhysicalDevice physicalDevice,
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

VkQueue VulkanCore::GetQueueFromDevice(VkDevice device, int queueIndex) {
  VkQueue queue;
  vkGetDeviceQueue(device, queueIndex, 0, &queue);
  return queue;
}

void VulkanCore::CreateSwapChain() {
}

void VulkanCore::CreateRenderPass() {
}

void VulkanCore::CreateDescriptorSetLayout() {
}

void VulkanCore::CreateGraphicsPipelineLayout() {
}

void VulkanCore::CreateShaderModule(const Vector<U8>& code) {
}

bool VkQueueIndices::IsComplete(const bool isTransferQueueRequired) const {
  const bool basicRequirement = m_graphicsFamily >= 0 && m_presentFamily >= 0;

  if (isTransferQueueRequired) {
    return basicRequirement && m_transferFamily >= 0;
  }

  return basicRequirement;
}
}
