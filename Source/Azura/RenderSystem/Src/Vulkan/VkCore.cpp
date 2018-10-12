#include "Vulkan/VkCore.h"
#include "Generic/Constants.h"

#include <array>
#include <map>
#include <set>
#include "Memory/MemoryFactory.h"
#include "Memory/MonotonicAllocator.h"
#include "Memory/StackMemoryBuffer.h"
#include "Memory/HeapMemoryBuffer.h"
#include "Vulkan/VkMacros.h"
#include "Vulkan/VkTypeMapping.h"
#include "Vulkan/VkScopedBuffer.h"
#include "Vulkan/VkScopedSwapChain.h"

#ifdef BUILD_DEBUG
#include <iostream>
#endif

using ContainerExtent = Azura::Containers::ContainerExtent;

namespace Azura {
namespace Vulkan {
namespace {
#ifdef BUILD_DEBUG
const std::array<const char*, 1> VALIDATION_LAYERS = {
  "VK_LAYER_LUNARG_standard_validation"
};

VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
  VkDebugReportFlagsEXT flags,
  VkDebugReportObjectTypeEXT objType,
  uint64_t obj,
  size_t location,
  int32_t code,
  const char* layerPrefix,
  const char* msg,
  void* userData) {
  UNUSED(userData);
  UNUSED(layerPrefix);
  UNUSED(code);
  UNUSED(location);
  UNUSED(obj);
  UNUSED(objType);
  UNUSED(flags);

  std::cout << "[VkRenderer] Validation layer: " << msg << std::endl;
  return VK_FALSE;
}

#endif

const std::array<const char*, 1> DEVICE_EXTENSIONS = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

bool CheckValidationLayerSupport() {
#ifdef BUILD_DEBUG
  HEAP_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 16384);
  U32 layerCount = 0;

  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  Containers::Vector<VkLayerProperties> availableLayers(ContainerExtent{layerCount}, allocatorTemporary);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.Data());

  for (const char* layer : VALIDATION_LAYERS) {
    bool layerFound = false;
    for (const auto& vkLayer : availableLayers) {
      if (strcmp(layer, &vkLayer.layerName[0]) == 0) {
        // NOLINT
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
  U32 extensionCount          = 0;
  const U32 maxExtensionCount = 128;

  HEAP_ALLOCATOR(Extension, Memory::MonotonicAllocator, sizeof(VkExtensionProperties) * maxExtensionCount)

  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
  Containers::Vector<VkExtensionProperties> availableExtensions(ContainerExtent{extensionCount, maxExtensionCount},
                                                                allocatorExtension);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.Data());

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

void LogDeviceFeatures(VkPhysicalDeviceFeatures features, const Log& log_VulkanRenderSystem) {
  UNUSED(features);
  UNUSED(log_VulkanRenderSystem);

  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Robust Buffer Access: %d", features.robustBufferAccess);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Full Draw Index Uint32: %d", features.fullDrawIndexUint32);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Image Cube Array: %d", features.imageCubeArray);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Independent Blend: %d", features.independentBlend);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Geometry Shader: %d", features.geometryShader);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Tessellation Shader: %d", features.tessellationShader);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Sample Rate Shading: %d", features.sampleRateShading);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Dual Src Blend: %d", features.dualSrcBlend);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Logic Op: %d", features.logicOp);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Multi Draw Indirect: %d", features.multiDrawIndirect);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Draw Indirect First Instance: %d", features.drawIndirectFirstInstance);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Depth Clamp: %d", features.depthClamp);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Depth Bias Clamp: %d", features.depthBiasClamp);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Fill Mode Non Solid: %d", features.fillModeNonSolid);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Depth Bounds: %d", features.depthBounds);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Wide Lines: %d", features.wideLines);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Large Points: %d", features.largePoints);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Alpha To One: %d", features.alphaToOne);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Multi Viewport: %d", features.multiViewport);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Sampler Anisotropy: %d", features.samplerAnisotropy);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Texture Compression ETC 2: %d", features.textureCompressionETC2);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Texture Compression ASTC LDR: %d", features.textureCompressionASTC_LDR);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "texture Compression BC: %d", features.textureCompressionBC);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Occlusion Query Precise: %d", features.occlusionQueryPrecise);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Pipeline Statistics Query: %d", features.pipelineStatisticsQuery);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Vertex Pipeline Stores And Atomics: %d", features.
    vertexPipelineStoresAndAtomics);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Fragment Stores And Atomics: %d", features.fragmentStoresAndAtomics);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Shader Tessellation And Geometry Point Size: %d", features.
    shaderTessellationAndGeometryPointSize);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Shader Image Gather Extended: %d", features.shaderImageGatherExtended);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Shader Storage Image Extended Formats: %d", features.
    shaderStorageImageExtendedFormats);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Shader Storage Image Multisample: %d", features.
    shaderStorageImageMultisample);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Shader StorageImageRead Without Format: %d", features.
    shaderStorageImageReadWithoutFormat);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Shader Storage ImageWrite Without Format: %d", features.
    shaderStorageImageWriteWithoutFormat);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Shader Uniform Buffer Array Dynamic Indexing: %d", features.
    shaderUniformBufferArrayDynamicIndexing);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Shader Sampled Image Array Dynamic Indexing: %d", features.
    shaderSampledImageArrayDynamicIndexing);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Shader Storage Buffer Array Dynamic Indexing: %d", features.
    shaderStorageBufferArrayDynamicIndexing);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Shader Storage Image Array Dynamic Indexing: %d", features.
    shaderStorageImageArrayDynamicIndexing);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Shader Clip Distance: %d", features.shaderClipDistance);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Shader Cull Distance: %d", features.shaderCullDistance);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Shader Float64: %d", features.shaderFloat64);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Shader Int64: %d", features.shaderInt64);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Shader Int16: %d", features.shaderInt16);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Shader Resource Residency: %d", features.shaderResourceResidency);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "ShaderResource Min Lod: %d", features.shaderResourceMinLod);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Sparse Binding: %d", features.sparseBinding);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Sparse Residency Buffer: %d", features.sparseResidencyBuffer);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Sparse Residency Image2 D: %d", features.sparseResidencyImage2D);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Sparse Residency Image3 D: %d", features.sparseResidencyImage3D);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Sparse Residency2 Samples: %d", features.sparseResidency2Samples);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Sparse Residency4 Samples: %d", features.sparseResidency4Samples);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Sparse Residency8 Samples: %d", features.sparseResidency8Samples);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Sparse Residency16 Samples: %d", features.sparseResidency16Samples);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Sparse Residency Aliased: %d", features.sparseResidencyAliased);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Variable Multisample Rate: %d", features.variableMultisampleRate);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Inherited Queries: %d", features.inheritedQueries);
}

int GetDeviceScore(VkPhysicalDevice device,
                   VkSurfaceKHR surface,
                   const DeviceRequirements& requirements,
                   const SwapChainDeviceSupport& swapChainSupport,
                   const Log& log_VulkanRenderSystem) {
  int score = 0;
  VkPhysicalDeviceProperties deviceProperties;
  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceProperties(device, &deviceProperties);
  vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Device Info: Device: %s", deviceProperties.deviceName);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Device ID: %d", deviceProperties.deviceID);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "API Version: %d", deviceProperties.apiVersion);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Driver Version: %d", deviceProperties.driverVersion);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Device Type: %s", VkPhysicalDeviceTypeToString(deviceProperties.deviceType
  ));
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Vendor ID: %d", deviceProperties.vendorID);

  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Device Features: Device: %s", deviceProperties.deviceName);
  LogDeviceFeatures(deviceFeatures, log_VulkanRenderSystem);

  const VkQueueIndices indices = VkCore::FindQueueFamiliesInDevice(device, surface, requirements,
                                                                   log_VulkanRenderSystem);
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
} // namespace

#ifdef BUILD_DEBUG
VkResult VkCore::CreateDebugReportCallbackEXT(VkInstance instance,
                                              const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
                                              const VkAllocationCallbacks* pAllocator,
                                              VkDebugReportCallbackEXT* pCallback) {
  // NOLINTNEXTLINE
  auto func = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
    vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pCallback);
  }

  return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void VkCore::DestroyDebugReportCallbackEXT(VkInstance instance,
                                           VkDebugReportCallbackEXT callback,
                                           const VkAllocationCallbacks* pAllocator) {
  // NOLINTNEXTLINE
  auto func = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
    vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));
  if (func != nullptr) {
    func(instance, callback, pAllocator);
  }
}


VkDebugReportCallbackEXT VkCore::SetupDebug(VkInstance instance, const Log& log_VulkanRenderSystem) {
  // Attach callback since we have validation turned on
  VkDebugReportCallbackCreateInfoEXT createInfo = {};
  createInfo.sType                              = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
  createInfo.flags                              = VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT;
  createInfo.pfnCallback                        = DebugReportCallback;

  VkDebugReportCallbackEXT callback;
  VERIFY_VK_OP(log_VulkanRenderSystem, CreateDebugReportCallbackEXT(instance, &createInfo, nullptr, &callback),
    "Failed to create Debug Callback");
  return callback;
}
#endif

VkInstance VkCore::CreateInstance(const ApplicationInfo& applicationData,
                                  const Containers::Vector<const char*>& vkExtensions,
                                  const Log& log_VulkanRenderSystem) {
  VkApplicationInfo appInfo = {};
  appInfo.sType             = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName  = applicationData.m_name.c_str();

  const U32 majorVer         = std::get<SemverMajor>(applicationData.m_version);
  const U32 minorVer         = std::get<SemverMinor>(applicationData.m_version);
  const U32 patchVer         = std::get<SemverPatch>(applicationData.m_version);
  appInfo.applicationVersion = VK_MAKE_VERSION(majorVer, minorVer, patchVer);

  appInfo.pEngineName   = "Azura";
  appInfo.engineVersion =
    VK_MAKE_VERSION(RENDER_SYSTEM_MAJOR_SEMVER, RENDER_SYSTEM_MINOR_SEMVER, RENDER_SYSTEM_PATCH_SEMVER);
  appInfo.apiVersion = VK_API_VERSION_1_1;

  VkInstanceCreateInfo createInfo    = {};
  createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo        = &appInfo;
  createInfo.enabledExtensionCount   = vkExtensions.GetSize();
  createInfo.ppEnabledExtensionNames = vkExtensions.Data();

  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Instance Creation: Loading Device Extensions");
  for (const auto& extension : vkExtensions) {
    // TODO(vasumahesh1):[PERF]: Check compiler optimization here
    UNUSED(extension);
    LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "= Instance Extensions: %s", extension);
  }

  // Validation Layers Check
  VERIFY_TRUE(log_VulkanRenderSystem, CheckValidationLayerSupport(),
    "Validation layers requested, but not available on device");

#ifdef BUILD_DEBUG
  createInfo.enabledLayerCount   = U32(VALIDATION_LAYERS.size());
  createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
#else
  createInfo.enabledLayerCount   = 0;
  createInfo.ppEnabledLayerNames = nullptr;
#endif

  VkInstance result;
  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateInstance(&createInfo, nullptr, &result), "Failed to create VkInstance");
  return result;
}

VkQueueIndices VkCore::FindQueueFamiliesInDevice(VkPhysicalDevice device,
                                                 VkSurfaceKHR surface,
                                                 const DeviceRequirements& requirements,
                                                 const Log& log_VulkanRenderSystem) {
  VkQueueIndices result;
  result.m_isTransferQueueRequired = requirements.m_transferQueue;

  UNUSED(log_VulkanRenderSystem);

  const U32 maxQueueFamilies = 4;
  STACK_ALLOCATOR(QueueFamily, Memory::MonotonicAllocator, sizeof(VkQueueFamilyProperties) * maxQueueFamilies)

  U32 queueFamilyCount;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
  Containers::Vector<VkQueueFamilyProperties> queueFamilies(ContainerExtent{queueFamilyCount, maxQueueFamilies},
                                                            allocatorQueueFamily);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.Data());

  int idx = 0;
  for (const auto& queueFamily : queueFamilies) {
    if (queueFamily.queueCount > 0 && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
      LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Found Graphics Queue At: %d", idx);
      result.m_graphicsFamily = idx;
    }

    if (queueFamily.queueCount > 0 && !((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
        && (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) {
      LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Found Transfer Queue At (was Required): %d", idx);
      result.m_transferFamily = idx;
    }

    VkBool32 canPresent = 0u;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, idx, surface, &canPresent);

    if (queueFamily.queueCount > 0 && canPresent != 0u) {
      LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Found Present Queue At: %d", idx);
      result.m_presentFamily = idx;
    }

    ++idx;
  }

  return result;
}

SwapChainDeviceSupport VkCore::QuerySwapChainSupport(VkPhysicalDevice device,
                                                     VkSurfaceKHR surface,
                                                     Memory::Allocator& allocator) {
  SwapChainDeviceSupport result(allocator);

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &result.m_capabilities);
  U32 formatCount = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

  if (formatCount > 0) {
    result.m_formats.Resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, result.m_formats.Data());
  }

  U32 presentCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentCount, nullptr);

  if (presentCount > 0) {
    result.m_presentModes.Resize(presentCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentCount, result.m_presentModes.Data());
  }

  return result;
}

VkPhysicalDevice VkCore::SelectPhysicalDevice(VkInstance instance,
                                              VkSurfaceKHR surface,
                                              const DeviceRequirements& requirements,
                                              const Log& log_VulkanRenderSystem) {
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

  // Query for Available Devices
  U32 availableDeviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &availableDeviceCount, nullptr);

  VERIFY_TRUE(log_VulkanRenderSystem, availableDeviceCount != 0, "No supported GPUs found with Vulkan");

  const U32 maxDevices = 4;
  STACK_ALLOCATOR(Device, Memory::MonotonicAllocator, sizeof(VkPhysicalDevice) * maxDevices);

  STACK_ALLOCATOR(SwapChainSupport, Memory::MonotonicAllocator, 2048);

  Containers::Vector<VkPhysicalDevice> availableDevices(ContainerExtent{availableDeviceCount, maxDevices},
                                                        allocatorDevice);
  vkEnumeratePhysicalDevices(instance, &availableDeviceCount, availableDevices.Data());

  std::multimap<int, VkPhysicalDevice> candidates;

  // Rate and Select the best GPU
  for (const auto& device : availableDevices) {
    const auto swapChainSupport = QuerySwapChainSupport(device, surface, allocatorSwapChainSupport);
    int tempScore               = GetDeviceScore(device, surface, requirements, swapChainSupport,
                                                 log_VulkanRenderSystem);
    candidates.insert(std::make_pair(tempScore, device));
  }

  if (candidates.rbegin()->first > 0) {
    physicalDevice = candidates.rbegin()->second;
  }

#ifdef BUILD_DEBUG
  VkPhysicalDeviceProperties deviceProperties;
  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
  vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "[SELECTED DEVICE] Device Info: Device: %s", deviceProperties.deviceName);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Device ID: %d", deviceProperties.deviceID);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "API Version: %d", deviceProperties.apiVersion);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Driver Version: %d", deviceProperties.driverVersion);
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Device Type: %s", VkPhysicalDeviceTypeToString(deviceProperties.deviceType
  ));
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Vendor ID: %d", deviceProperties.vendorID);

  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Device Features: Device: %s", deviceProperties.deviceName);
  LogDeviceFeatures(deviceFeatures, log_VulkanRenderSystem);
#endif

  VERIFY_TRUE(log_VulkanRenderSystem, physicalDevice != VK_NULL_HANDLE, "No suitable GPU found for Vulkan");
  return physicalDevice;
}

VkDevice VkCore::CreateLogicalDevice(VkPhysicalDevice physicalDevice,
                                     const VkQueueIndices& queueIndices,
                                     const DeviceRequirements& requirements,
                                     const Log& log_VulkanRenderSystem) {
  // Create Queue Infos
  const std::set<int> uniqueQueueFamilies = {
    queueIndices.m_graphicsFamily, queueIndices.m_presentFamily,
    queueIndices.m_transferFamily
  };

  const U32 maxQueueFamilies = 3;
  STACK_ALLOCATOR(QueueCreateInfo, Memory::MonotonicAllocator, sizeof(VkDeviceQueueCreateInfo) * maxQueueFamilies)

  // TODO(vasumahesh1): Simplify with Azura::Vector
  Containers::Vector<VkDeviceQueueCreateInfo> queueCreateInfos(maxQueueFamilies, allocatorQueueCreateInfo);

  float queuePriority = 1.0f;

  for (const auto& queueIdx : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex        = queueIdx;
    queueCreateInfo.queueCount              = 1;
    queueCreateInfo.pQueuePriorities        = &queuePriority;
    queueCreateInfos.PushBack(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures = {};
  deviceFeatures.shaderFloat64            = requirements.m_float64 ? VK_TRUE : VK_FALSE;
  deviceFeatures.shaderInt64              = requirements.m_int64 ? VK_TRUE : VK_FALSE;

  // Create the logical device
  VkDeviceCreateInfo createInfo      = {};
  createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos       = queueCreateInfos.Data();
  createInfo.queueCreateInfoCount    = queueCreateInfos.GetSize();
  createInfo.pEnabledFeatures        = &deviceFeatures;
  createInfo.enabledExtensionCount   = U32(DEVICE_EXTENSIONS.size());
  createInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Device Creation: Loading Device Extensions");
  for (const auto& extension : DEVICE_EXTENSIONS) {
    // TODO(vasumahesh1):[PERF]: Check compiler optimization here
    UNUSED(extension);
    LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "= Device Extensions: %s", extension);
  }

#ifdef BUILD_DEBUG
  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Device Creation: Loading Device Validation Layers");
  for (const auto& layer : VALIDATION_LAYERS) {
    LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "= Validation Layer: %s", layer);
  }

  createInfo.enabledLayerCount   = U32(VALIDATION_LAYERS.size());
  createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
#else
  createInfo.enabledLayerCount   = 0;
  createInfo.ppEnabledLayerNames = nullptr;
#endif

  VkDevice device;
  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateDevice(physicalDevice, &createInfo, nullptr, &device),
    "Failed to create VkDevice");

  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Created VkDevice Successfully");
  return device;
}

VkQueue VkCore::GetQueueFromDevice(VkDevice device, int queueIndex) {
  VkQueue queue;
  vkGetDeviceQueue(device, queueIndex, 0, &queue);
  return queue;
}

VkImage VkCore::CreateImage(VkDevice device,
                            RawStorageFormat format,
                            ImageType imageType,
                            const Bounds2D& bounds,
                            U32 depth,
                            U32 layers,
                            U32 mips,
                            VkImageTiling tiling,
                            VkImageUsageFlags imageUsage,
                            const Log& log_VulkanRenderSystem) {
  const auto imageFormat = ToVkFormat(format);
  VERIFY_OPT(log_VulkanRenderSystem, imageFormat, "Unknown Format");

  const auto vkImageType = ToVkImageType(imageType);
  VERIFY_OPT(log_VulkanRenderSystem, vkImageType, "Unknown Image Type");

  assert(bounds.m_width > 0);
  assert(bounds.m_height > 0);
  assert(depth > 0);
  assert(layers > 0);
  assert(mips > 0);

  VkImageCreateInfo createInfo = {};
  createInfo.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  createInfo.imageType         = vkImageType.value();
  // Note that vkCmdBlitImage (if supported) will also do format conversions if the swapchain color format would differ
  createInfo.format        = imageFormat.value();
  createInfo.extent.width  = bounds.m_width;
  createInfo.extent.height = bounds.m_height;
  createInfo.extent.depth  = depth;
  createInfo.arrayLayers   = layers;
  createInfo.mipLevels     = mips;
  createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  createInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
  createInfo.tiling        = tiling;
  createInfo.usage         = imageUsage;
  createInfo.flags         = 0;

  // TODO(vasumahesh1):[TEXTURE]: Add support for Cube Maps

  VkImage resultImage;
  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateImage(device, &createInfo, nullptr, &resultImage),
    "Failed to Create Image");
  return resultImage;
}

VkImageView VkCore::CreateImageView(VkDevice device,
                                    VkImage sourceImage,
                                    VkImageViewType viewType,
                                    VkFormat viewFormat,
                                    VkImageAspectFlags aspectMask,
                                    const Log& log_VulkanRenderSystem,
                                    U32 baseMip,
                                    U32 levelCount,
                                    U32 baseArrayLayer,
                                    U32 layerCount) {
  VkImageViewCreateInfo createInfo = {};
  createInfo.sType                 = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  createInfo.image                 = sourceImage;
  createInfo.viewType              = viewType;
  createInfo.format                = viewFormat;

  // TODO(vasumahesh1):[TEXTURE]: Add Swizzle support
  // Default Swizzle
  createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

  // Which part of image to access
  createInfo.subresourceRange.aspectMask     = aspectMask;
  createInfo.subresourceRange.baseMipLevel   = baseMip;
  createInfo.subresourceRange.levelCount     = levelCount;
  createInfo.subresourceRange.baseArrayLayer = baseArrayLayer;
  createInfo.subresourceRange.layerCount     = layerCount;

  VkImageView imageView;
  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateImageView(device, &createInfo, nullptr, &imageView),
    "Failed to create image view");
  return imageView;
}

// TODO(vasumahesh1):[PIPELINE]: Needs serious changes here
VkRenderPass VkCore::CreateRenderPass(VkDevice device,
                                      const VkScopedSwapChain& swapChain,
                                      const Log& log_VulkanRenderSystem) {
  VkAttachmentDescription colorAttachment = {};
  colorAttachment.format                  = swapChain.GetSurfaceFormat();
  colorAttachment.samples                 = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp                 = VK_ATTACHMENT_STORE_OP_STORE;

  // Not using Stencil buffer
  colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout   = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentDescription depthAttachment = {};
  depthAttachment.format                  = swapChain.GetDepthFormat();
  depthAttachment.samples                 = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp                 = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp           = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp          = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout             = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment            = 0;
  colorAttachmentRef.layout                = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentRef = {};
  depthAttachmentRef.attachment            = 1;
  depthAttachmentRef.layout                = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass    = {};
  subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount    = 1;
  subpass.pColorAttachments       = &colorAttachmentRef;
  subpass.pDepthStencilAttachment = &depthAttachmentRef;

  std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount        = U32(attachments.size());
  renderPassInfo.pAttachments           = attachments.data();
  renderPassInfo.subpassCount           = 1;
  renderPassInfo.pSubpasses             = &subpass;

  // Tell special subpass to wait for Image acquisition from semaphore
  VkSubpassDependency dependency = {};
  dependency.srcSubpass          = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass          = 0;
  dependency.srcStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask       = 0;
  dependency.dstStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies   = &dependency;

  VkRenderPass renderPass;
  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass),
    "Failed to create render pass");
  return renderPass;
}

void VkCore::CreateUniformBufferBinding(Containers::Vector<VkDescriptorSetLayoutBinding>& bindings,
                                        const U32 binding,
                                        const U32 count,
                                        VkShaderStageFlags stageFlag) {
  VkDescriptorSetLayoutBinding uboLayoutBinding = {};
  uboLayoutBinding.binding                      = binding;
  uboLayoutBinding.descriptorType               = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.descriptorCount              = count;
  uboLayoutBinding.stageFlags                   = stageFlag;
  uboLayoutBinding.pImmutableSamplers           = nullptr;

  bindings.PushBack(std::move(uboLayoutBinding));
}

void VkCore::CreateSamplerBinding(Containers::Vector<VkDescriptorSetLayoutBinding>& bindings,
                                  U32 binding,
                                  U32 count,
                                  VkShaderStageFlags stageFlag) {
  VkDescriptorSetLayoutBinding uboLayoutBinding = {};
  uboLayoutBinding.binding                      = binding;
  uboLayoutBinding.descriptorType               = VK_DESCRIPTOR_TYPE_SAMPLER;
  uboLayoutBinding.descriptorCount              = count;
  uboLayoutBinding.stageFlags                   = stageFlag;
  uboLayoutBinding.pImmutableSamplers           = nullptr;

  bindings.PushBack(std::move(uboLayoutBinding));
}

void VkCore::CreateSampledImageBinding(Containers::Vector<VkDescriptorSetLayoutBinding>& bindings,
                                       U32 binding,
                                       U32 count,
                                       VkShaderStageFlags stageFlag) {
  VkDescriptorSetLayoutBinding uboLayoutBinding = {};
  uboLayoutBinding.binding                      = binding;
  uboLayoutBinding.descriptorType               = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
  uboLayoutBinding.descriptorCount              = count;
  uboLayoutBinding.stageFlags                   = stageFlag;
  uboLayoutBinding.pImmutableSamplers           = nullptr;

  bindings.PushBack(std::move(uboLayoutBinding));
}

void VkCore::CreateCombinedImageSamplerBinding(Containers::Vector<VkDescriptorSetLayoutBinding>& bindings,
                                               U32 binding,
                                               U32 count,
                                               VkShaderStageFlags stageFlag) {
  VkDescriptorSetLayoutBinding uboLayoutBinding = {};
  uboLayoutBinding.binding                      = binding;
  uboLayoutBinding.descriptorType               = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  uboLayoutBinding.descriptorCount              = count;
  uboLayoutBinding.stageFlags                   = stageFlag;
  uboLayoutBinding.pImmutableSamplers           = nullptr;

  bindings.PushBack(std::move(uboLayoutBinding));
}


VkDescriptorSetLayout VkCore::CreateDescriptorSetLayout(
  VkDevice device,
  const Containers::Vector<VkDescriptorSetLayoutBinding>& bindings,
  const Log& log_VulkanRenderSystem) {
  VkDescriptorSetLayoutCreateInfo layoutInfo = {};
  layoutInfo.sType                           = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount                    = bindings.GetSize();
  layoutInfo.pBindings                       = bindings.Data();

  VkDescriptorSetLayout descriptorSet;
  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSet),
    "Failed to create descriptor set layout");
  return descriptorSet;
}

VkPipelineLayout VkCore::CreatePipelineLayout(VkDevice device,
                                              const Containers::Vector<VkDescriptorSetLayout>& descriptorSets,
                                              const Log& log_VulkanRenderSystem) {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.sType                      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

  // Descriptors Info
  pipelineLayoutInfo.pSetLayouts    = descriptorSets.Data();
  pipelineLayoutInfo.setLayoutCount = descriptorSets.GetSize();

  VkPipelineLayout pipelineLayout;
  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout),
    "Failed to create pipeline layout");
  return pipelineLayout;
}

VkShaderModule VkCore::CreateShaderModule(VkDevice device,
                                          const Containers::Vector<U8>& code,
                                          const Log& log_VulkanRenderSystem) {
  VkShaderModuleCreateInfo createInfo = {};
  createInfo.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize                 = code.GetSize();
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  createInfo.pCode = reinterpret_cast<const U32*>(code.Data()); // byte code ptr conversion

  VkShaderModule shaderModule;
  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule),
    "Failed to create shader module");
  return shaderModule;
}

Containers::Vector<VkFramebuffer> VkCore::CreateFrameBuffers(VkDevice device,
                                                             VkRenderPass renderPass,
                                                             const VkScopedSwapChain& scopedSwapChain,
                                                             Memory::Allocator& allocator,
                                                             const Log& log_VulkanRenderSystem) {
  const auto& allImages      = scopedSwapChain.GetAllImages();
  const auto swapChainExtent = scopedSwapChain.GetExtent();

  Containers::Vector<VkFramebuffer> frameBuffers(ContainerExtent{allImages.GetSize()}, allocator);

  for (U32 idx = 0; idx < allImages.GetSize(); ++idx) {

    const std::array<VkImageView, 1> attachments = {allImages[idx].View()};

    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass              = renderPass;
    framebufferInfo.attachmentCount         = U32(attachments.size());
    framebufferInfo.pAttachments            = attachments.data();
    framebufferInfo.width                   = swapChainExtent.width;
    framebufferInfo.height                  = swapChainExtent.height;
    framebufferInfo.layers                  = 1;

    VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateFramebuffer(device, &framebufferInfo, nullptr, &frameBuffers[idx]),
      "Failed to create framebuffer");
  }

  return frameBuffers;
}

void VkCore::CreateFrameBuffers(VkDevice device,
                                VkRenderPass renderPass,
                                const VkScopedSwapChain& scopedSwapChain,
                                Containers::Vector<VkFramebuffer>& frameBuffers,
                                const Log& log_VulkanRenderSystem) {

  const auto& allImages      = scopedSwapChain.GetAllImages();
  const auto swapChainExtent = scopedSwapChain.GetExtent();

  frameBuffers.Resize(allImages.GetSize());

  VkImageView depthImageView = scopedSwapChain.GetDepthImage().View();

  for (U32 idx = 0; idx < allImages.GetSize(); ++idx) {

    const std::array<VkImageView, 2> attachments = {allImages[idx].View(), depthImageView};

    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass              = renderPass;
    framebufferInfo.attachmentCount         = U32(attachments.size());
    framebufferInfo.pAttachments            = attachments.data();
    framebufferInfo.width                   = swapChainExtent.width;
    framebufferInfo.height                  = swapChainExtent.height;
    framebufferInfo.layers                  = 1;

    VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateFramebuffer(device, &framebufferInfo, nullptr, &frameBuffers[idx]),
      "Failed to create framebuffer");
  }
}

VkCommandPool VkCore::CreateCommandPool(VkDevice device,
                                        int queueIndex,
                                        VkCommandPoolCreateFlags flags,
                                        const Log& log_VulkanRenderSystem) {
  VkCommandPoolCreateInfo poolInfo = {};
  poolInfo.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.queueFamilyIndex        = queueIndex;
  poolInfo.flags                   = flags;

  VkCommandPool commandPool;
  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool),
    "Failed to create command pool");
  return commandPool;
}

U32 VkCore::FindMemoryType(U32 typeFilter,
                           VkMemoryPropertyFlags properties,
                           const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties) {
  // TODO(vasumahesh1): Fix this warning
  for (U32 idx = 0; idx < physicalDeviceMemoryProperties.memoryTypeCount; ++idx) {
    if (((typeFilter & (1 << idx)) != 0u)
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
        && ((physicalDeviceMemoryProperties.memoryTypes[idx].propertyFlags & properties) == properties)) {
      return idx;
    }
  }

  throw std::runtime_error("Failed to find suitable memory type");
}

VkDescriptorPoolSize VkCore::CreateDescriptorPoolSize(VkDescriptorType type, U32 descriptorCount) {
  VkDescriptorPoolSize poolSize;
  poolSize.type            = type;
  poolSize.descriptorCount = descriptorCount;
  return poolSize;
}

VkDescriptorPool VkCore::CreateDescriptorPool(VkDevice device,
                                              const Containers::Vector<VkDescriptorPoolSize>& pools,
                                              U32 maxSets,
                                              const Log& log_VulkanRenderSystem) {
  VkDescriptorPoolCreateInfo poolInfo = {};
  poolInfo.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount              = pools.GetSize();
  poolInfo.pPoolSizes                 = pools.Data();
  poolInfo.maxSets                    = maxSets;

  VkDescriptorPool descriptorPool;
  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool),
    "Failed to create descriptor pool");
  return descriptorPool;
}

VkDescriptorSet VkCore::CreateDescriptorSet(VkDevice device,
                                            VkDescriptorPool descriptorPool,
                                            const Containers::Vector<VkDescriptorSetLayout>& descriptorSets,
                                            const Log& log_VulkanRenderSystem) {
  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType                       = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool              = descriptorPool;
  allocInfo.descriptorSetCount          = descriptorSets.GetSize();
  allocInfo.pSetLayouts                 = descriptorSets.Data();

  VkDescriptorSet descriptorSet;
  VERIFY_VK_OP(log_VulkanRenderSystem, vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet),
    "Failed to allocate descriptor set");
  return descriptorSet;
}

VkWriteDescriptorSet VkCore::CreateWriteDescriptorForUniformBuffer(
  VkDescriptorSet set,
  U32 layoutIndex,
  U32 binding,
  const Containers::Vector<VkDescriptorBufferInfo>& bufferInfos) {
  VkWriteDescriptorSet descriptorWrite = {};
  descriptorWrite.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrite.dstSet               = set;
  descriptorWrite.dstArrayElement      = layoutIndex;
  descriptorWrite.dstBinding           = binding;
  descriptorWrite.descriptorType       = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

  // Attach Buffer Info to WriteDescriptorSet
  descriptorWrite.descriptorCount  = bufferInfos.GetSize();
  descriptorWrite.pBufferInfo      = bufferInfos.Data();
  descriptorWrite.pImageInfo       = nullptr;
  descriptorWrite.pTexelBufferView = nullptr;

  return descriptorWrite;
}

void VkCore::UpdateDescriptorSets(VkDevice device, const Containers::Vector<VkWriteDescriptorSet>& descriptorWrites) {
  vkUpdateDescriptorSets(device, U32(descriptorWrites.GetSize()), descriptorWrites.Data(), 0, nullptr);
}

VkCommandBuffer VkCore::CreateCommandBuffer(VkDevice device,
                                            VkCommandPool commandPool,
                                            VkCommandBufferLevel level,
                                            const Log& log_VulkanRenderSystem) {
  VkCommandBuffer commandBuffer;

  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool                 = commandPool;
  allocInfo.level                       = level;
  allocInfo.commandBufferCount          = 1;

  VERIFY_VK_OP(log_VulkanRenderSystem, vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer),
    "Failed to allocate command buffers");

  return commandBuffer;
}

Containers::Vector<VkCommandBuffer> VkCore::CreateCommandBuffers(
  VkDevice device,
  U32 count,
  VkCommandPool commandPool,
  VkCommandBufferLevel level,
  Memory::Allocator& allocator,
  const Log& log_VulkanRenderSystem) {
  Containers::Vector<VkCommandBuffer> commandBuffers(ContainerExtent{count}, allocator);
  CreateCommandBuffers(device, commandPool, level, commandBuffers, log_VulkanRenderSystem);
  return commandBuffers;
}

void VkCore::CreateCommandBuffers(VkDevice device,
                                  VkCommandPool commandPool,
                                  VkCommandBufferLevel level,
                                  Containers::Vector<VkCommandBuffer>& commandBuffers,
                                  const Log& log_VulkanRenderSystem) {
  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool                 = commandPool;
  allocInfo.level                       = level;
  allocInfo.commandBufferCount          = commandBuffers.GetSize();

  VERIFY_VK_OP(log_VulkanRenderSystem, vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.Data()),
    "Failed to create command buffers");
}

void VkCore::BeginCommandBuffer(VkCommandBuffer buffer,
                                VkCommandBufferUsageFlags flags,
                                const Log& log_VulkanRenderSystem) {
  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags                    = flags;
  beginInfo.pInheritanceInfo         = nullptr;

  VERIFY_VK_OP(log_VulkanRenderSystem, vkBeginCommandBuffer(buffer, &beginInfo),
    "Failed to begin recording command buffer");
}

void VkCore::BeginCommandBuffer(VkCommandBuffer buffer,
                                VkCommandBufferUsageFlags flags,
                                const VkCommandBufferInheritanceInfo& inheritanceInfo,
                                const Log& log_VulkanRenderSystem) {
  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags                    = flags;
  beginInfo.pInheritanceInfo         = &inheritanceInfo;

  VERIFY_VK_OP(log_VulkanRenderSystem, vkBeginCommandBuffer(buffer, &beginInfo),
    "Failed to begin recording command buffer");
}

void VkCore::EndCommandBuffer(VkCommandBuffer buffer, const Log& log_VulkanRenderSystem) {
  VERIFY_VK_OP(log_VulkanRenderSystem, vkEndCommandBuffer(buffer), "Failed to end recording command buffer");
}

VkSemaphore VkCore::CreateSemaphore(VkDevice device, const Log& log_VulkanRenderSystem) {
  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  VkSemaphore semaphore;
  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore),
    "Failed to create semaphore");
  return semaphore;
}

void VkCore::CreateSemaphores(VkDevice device,
                              U32 count,
                              Containers::Vector<VkSemaphore>& semaphores,
                              const Log& log_VulkanRenderSystem) {
  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  for (U32 idx = 0; idx < count; ++idx) {
    VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphores[idx]),
      "Failed to create semaphore [multi-create]");
  }
}

Containers::Vector<VkSemaphore> VkCore::CreateSemaphores(VkDevice device,
                                                         U32 count,
                                                         Memory::Allocator& allocator,
                                                         const Log& log_VulkanRenderSystem) {
  Containers::Vector<VkSemaphore> semaphores(ContainerExtent{count}, allocator);
  CreateSemaphores(device, count, semaphores, log_VulkanRenderSystem);
  return semaphores;
}

VkFence VkCore::CreateFence(VkDevice device, VkFenceCreateFlags flags, const Log& log_VulkanRenderSystem) {
  VkFenceCreateInfo fenceInfo = {};
  fenceInfo.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags             = flags;

  VkFence fence;
  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateFence(device, &fenceInfo, nullptr, &fence), "Failed to create fence");
  return fence;
}

void VkCore::CreateFences(VkDevice device,
                          U32 count,
                          VkFenceCreateFlags flags,
                          Containers::Vector<VkFence>& fences,
                          const Log& log_VulkanRenderSystem) {
  VkFenceCreateInfo fenceInfo = {};
  fenceInfo.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags             = flags;

  for (U32 idx = 0; idx < count; ++idx) {
    VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateFence(device, &fenceInfo, nullptr, &fences[idx]),
      "Failed to create fences [multi-create]");
  }
}

Containers::Vector<VkFence> VkCore::CreateFences(VkDevice device,
                                                 U32 count,
                                                 VkFenceCreateFlags flags,
                                                 Memory::Allocator& allocator,
                                                 const Log& log_VulkanRenderSystem) {
  Containers::Vector<VkFence> fences(ContainerExtent{count}, allocator);
  CreateFences(device, count, flags, fences, log_VulkanRenderSystem);
  return fences;
}

void VkCore::CopyBuffer(VkDevice device,
                        VkQueue queue,
                        const VkScopedBuffer& srcBuffer,
                        const VkScopedBuffer& dstBuffer,
                        const VkDeviceSize size,
                        const VkCommandPool commandPool) {
  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool                 = commandPool;
  allocInfo.commandBufferCount          = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  VkBufferCopy copyRegion = {};
  copyRegion.srcOffset    = 0;
  copyRegion.dstOffset    = 0;
  copyRegion.size         = size;
  vkCmdCopyBuffer(commandBuffer, srcBuffer.Real(), dstBuffer.Real(), 1, &copyRegion);
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo       = {};
  submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers    = &commandBuffer;

  vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(queue);

  vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void VkCore::TransitionImageLayout(
  VkCommandBuffer cmdBuffer,
  VkImage image,
  VkAccessFlags srcAccessMask,
  VkAccessFlags dstAccessMask,
  VkImageLayout oldImageLayout,
  VkImageLayout newImageLayout,
  VkPipelineStageFlags srcStageMask,
  VkPipelineStageFlags dstStageMask,
  VkImageSubresourceRange imageSubresourceRange) {

  VkImageMemoryBarrier imageMemoryBarrier{};
  imageMemoryBarrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  imageMemoryBarrier.srcAccessMask       = srcAccessMask;
  imageMemoryBarrier.dstAccessMask       = dstAccessMask;
  imageMemoryBarrier.oldLayout           = oldImageLayout;
  imageMemoryBarrier.newLayout           = newImageLayout;
  imageMemoryBarrier.image               = image;
  imageMemoryBarrier.subresourceRange    = imageSubresourceRange;

  vkCmdPipelineBarrier(
                       cmdBuffer,
                       srcStageMask,
                       dstStageMask,
                       0,
                       0, nullptr,
                       0, nullptr,
                       1, &imageMemoryBarrier
                      );
}


void VkCore::ImageBlit(VkCommandBuffer cmdBuffer,
                       VkImage srcImage,
                       VkImage dstImage,
                       VkImageAspectFlagBits srcAspect,
                       VkImageAspectFlagBits dstAspect,
                       const Bounds3D& blitRegion,
                       const LayerSubresource& srcLayerResource,
                       const LayerSubresource& dstLayerResource,
                       U32 srcMipLevel,
                       U32 dstMipLevel,
                       VkFilter blitFilter) {
  VkOffset3D blitSize;
  blitSize.x = blitRegion.m_width;
  blitSize.y = blitRegion.m_height;
  blitSize.z = blitRegion.m_depth;

  VkImageBlit imageBlitRegion{};
  imageBlitRegion.srcSubresource.aspectMask     = srcAspect;
  imageBlitRegion.srcSubresource.layerCount     = srcLayerResource.m_layerCount;
  imageBlitRegion.srcSubresource.baseArrayLayer = srcLayerResource.m_baseLayer;
  imageBlitRegion.srcSubresource.mipLevel       = srcMipLevel;
  imageBlitRegion.srcOffsets[1]                 = blitSize;

  imageBlitRegion.dstSubresource.aspectMask     = dstAspect;
  imageBlitRegion.dstSubresource.layerCount     = dstLayerResource.m_layerCount;
  imageBlitRegion.dstSubresource.baseArrayLayer = dstLayerResource.m_baseLayer;
  imageBlitRegion.dstSubresource.mipLevel       = dstMipLevel;
  imageBlitRegion.dstOffsets[1]                 = blitSize;

  vkCmdBlitImage(
                 cmdBuffer,
                 srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                 dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                 1,
                 &imageBlitRegion,
                 blitFilter
                );
}

void VkCore::ImageBlit(VkCommandBuffer cmdBuffer,
                       VkImage srcImage,
                       VkImage dstImage,
                       VkImageAspectFlagBits srcAspect,
                       VkImageAspectFlagBits dstAspect,
                       const Bounds3D& blitRegion) {

  return ImageBlit(cmdBuffer, srcImage, dstImage, srcAspect, dstAspect, blitRegion, LayerSubresource{1, 0},
                   LayerSubresource{1, 0}, 0, 0, VK_FILTER_NEAREST);
}

void VkCore::ImageCopy(
  VkCommandBuffer cmdBuffer,
  VkImage srcImage,
  VkImage dstImage,
  const Containers::Vector<VkImageCopy>& copyRegions
) {
  vkCmdCopyImage(
                 cmdBuffer,
                 srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                 dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                 copyRegions.GetSize(),
                 copyRegions.Data());
}

VkImageCopy VkCore::GetImageCopyRegion(
  VkImageAspectFlagBits srcAspect,
  VkImageAspectFlagBits dstAspect,
  const Bounds3D& copyRegion,
  const TextureSubresource& srcSubresource,
  const TextureSubresource& dstSubresource) {
  VkImageCopy imageCopyRegion                   = {};
  imageCopyRegion.srcSubresource.aspectMask     = srcAspect;
  imageCopyRegion.srcSubresource.layerCount     = srcSubresource.m_layerInfo.m_layerCount;
  imageCopyRegion.srcSubresource.baseArrayLayer = srcSubresource.m_layerInfo.m_baseLayer;
  imageCopyRegion.srcSubresource.mipLevel       = srcSubresource.m_mipLevel;

  imageCopyRegion.dstSubresource.aspectMask     = dstAspect;
  imageCopyRegion.dstSubresource.layerCount     = dstSubresource.m_layerInfo.m_layerCount;
  imageCopyRegion.dstSubresource.baseArrayLayer = dstSubresource.m_layerInfo.m_baseLayer;
  imageCopyRegion.dstSubresource.mipLevel       = dstSubresource.m_mipLevel;

  imageCopyRegion.extent.width  = copyRegion.m_width;
  imageCopyRegion.extent.height = copyRegion.m_height;
  imageCopyRegion.extent.depth  = copyRegion.m_depth;

  return imageCopyRegion;
}

void VkCore::ImageCopy(
  VkCommandBuffer cmdBuffer,
  VkImage srcImage,
  VkImage dstImage,
  VkImageAspectFlagBits srcAspect,
  VkImageAspectFlagBits dstAspect,
  const Bounds3D& copyRegion,
  const TextureSubresource& srcSubresource,
  const TextureSubresource& dstSubresource
) {
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 1024);
  Containers::Vector<VkImageCopy> imageCopyRegions(1, allocatorTemporary);
  imageCopyRegions.PushBack(GetImageCopyRegion(srcAspect, dstAspect, copyRegion, srcSubresource, dstSubresource));

  ImageCopy(cmdBuffer, srcImage, dstImage, imageCopyRegions);
}

void VkCore::ImageCopy(
  VkCommandBuffer cmdBuffer,
  VkImage srcImage,
  VkImage dstImage,
  VkImageAspectFlagBits srcAspect,
  VkImageAspectFlagBits dstAspect,
  const Bounds3D& copyRegion
) {
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 1024);
  Containers::Vector<VkImageCopy> imageCopyRegions(1, allocatorTemporary);
  imageCopyRegions.PushBack(GetImageCopyRegion(srcAspect, dstAspect, copyRegion, {}, {}));

  ImageCopy(cmdBuffer, srcImage, dstImage, imageCopyRegions);
}

void VkCore::FlushCommandBuffer(VkDevice device,
                                VkCommandBuffer cmdBuffer,
                                VkQueue queue,
                                const Log& log_VulkanRenderSystem) {
  EndCommandBuffer(cmdBuffer, log_VulkanRenderSystem);

  VkSubmitInfo submitInfo       = {};
  submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers    = &cmdBuffer;

  VkFence fence = CreateFence(device, VK_FENCE_CREATE_SIGNALED_BIT, log_VulkanRenderSystem);
  vkResetFences(device, 1, &fence);

  // Submit to the queue
  VERIFY_VK_OP(log_VulkanRenderSystem, vkQueueSubmit(queue, 1, &submitInfo, fence),
    "Flush Command Buffer: Failed to submit to queue");

  // Wait for the fence to signal that command buffer has finished executing
  VERIFY_VK_OP(log_VulkanRenderSystem, vkWaitForFences(device, 1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max())
    , "Wait Fence failed");

  vkDestroyFence(device, fence, nullptr);
}

bool VkCore::QueryFormatFeatureSupport(VkPhysicalDevice physicalDevice,
                                       VkFormat format,
                                       std::function<bool(const VkFormatProperties&)> queryFunction) {
  VkFormatProperties formatProps;
  vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProps);

  return queryFunction(formatProps);
}

VkFormat VkCore::GetVkFormat(RawStorageFormat rawFormat, const Log& log_VulkanRenderSystem) {
  const auto format = ToVkFormat(rawFormat);
  VERIFY_OPT(log_VulkanRenderSystem, format, "Unknown Format");
  return format.value();
}

} // namespace Vulkan
} // namespace Azura
