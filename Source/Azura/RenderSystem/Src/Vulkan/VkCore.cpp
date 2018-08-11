#include "Vulkan/VkCore.h"
#include "Generic/Constants.h"

#include <array>
#include <map>
#include <set>
#include "Utils/Macros.h"
#include "Vulkan/VkTypeMapping.h"
#include "Vulkan/VkMacros.h"
#include "Vulkan/VkScopedBuffer.h"

namespace Azura {
namespace Vulkan {
namespace {
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

VkScopedSwapChain VkCore::CreateSwapChain(VkDevice device,
                                          VkSurfaceKHR surface,
                                          const VkQueueIndices& queueIndices,
                                          const SwapChainDeviceSupport& swapChainSupport,
                                          const SwapChainRequirement& swapChainRequirement) {

  VkScopedSwapChain scopedSwapChain;

  const VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.m_presentModes);
  scopedSwapChain.m_surfaceFormat    = ChooseSwapSurfaceFormat(swapChainSupport.m_formats, swapChainRequirement);
  scopedSwapChain.m_extent           = ChooseSwapExtent(swapChainSupport.m_capabilities, swapChainRequirement);

  // TODO(vasumahesh1): Need requirement?
  // Set Queue Length of SwapChain
  scopedSwapChain.m_imageCount = swapChainSupport.m_capabilities.minImageCount + 1;
  if (swapChainSupport.m_capabilities.maxImageCount > 0 && scopedSwapChain.m_imageCount > swapChainSupport
                                                                                          .m_capabilities.maxImageCount
  ) {
    scopedSwapChain.m_imageCount = swapChainSupport.m_capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo = {};
  createInfo.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface                  = surface;
  createInfo.minImageCount            = scopedSwapChain.m_imageCount;
  createInfo.imageFormat              = scopedSwapChain.m_surfaceFormat.format;
  createInfo.imageColorSpace          = scopedSwapChain.m_surfaceFormat.colorSpace;
  createInfo.imageExtent              = scopedSwapChain.m_extent;
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

  VERIFY_VK_OP(vkCreateSwapchainKHR(device, &createInfo, nullptr, &scopedSwapChain.m_swapChain),
    "Failed to create swap chain");

  vkGetSwapchainImagesKHR(device, scopedSwapChain.m_swapChain, &scopedSwapChain.m_imageCount, nullptr);
  scopedSwapChain.m_images.resize(scopedSwapChain.m_imageCount);
  vkGetSwapchainImagesKHR(device, scopedSwapChain.m_swapChain, &scopedSwapChain.m_imageCount,
                          scopedSwapChain.m_images.data());

  for (U32 idx = 0; idx < scopedSwapChain.m_imageCount; ++idx) {
    CreateImageView(device, scopedSwapChain.m_images[idx], VK_IMAGE_VIEW_TYPE_2D,
                    scopedSwapChain.m_surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
  }

  return scopedSwapChain;
}

VkImageView VkCore::CreateImageView(VkDevice device,
                                    VkImage sourceImage,
                                    VkImageViewType viewType,
                                    VkFormat viewFormat,
                                    VkImageAspectFlags aspectMask,
                                    U32 baseMip,
                                    U32 levelCount,
                                    U32 baseArrayLayer,
                                    U32 layerCount) {
  VkImageViewCreateInfo createInfo = {};
  createInfo.sType                 = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  createInfo.image                 = sourceImage;
  createInfo.viewType              = viewType;
  createInfo.format                = viewFormat;

  // TODO(vasumahesh1): Add Swizzle support
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
  VERIFY_VK_OP(vkCreateImageView(device, &createInfo, nullptr, &imageView), "Failed to create image view");
  return imageView;
}

// TODO(vasumahesh1): Needs serious changes here
VkRenderPass VkCore::CreateRenderPass(VkDevice device, VkFormat colorFormat) {
  VkAttachmentDescription colorAttachment = {};
  colorAttachment.format                  = colorFormat;
  colorAttachment.samples                 = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp                 = VK_ATTACHMENT_STORE_OP_STORE;

  // Not using Stencil buffer
  colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout   = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment            = 0;
  colorAttachmentRef.layout                = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments    = &colorAttachmentRef;

  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount        = 1;
  renderPassInfo.pAttachments           = &colorAttachment;
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
  VERIFY_VK_OP(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass), "Failed to create render pass");
  return renderPass;
}

void VkCore::BindUniformBufferToDescriptorSet(std::vector<VkDescriptorSetLayoutBinding>& bindings,
                                              VkShaderStageFlags stageFlag) {

  VkDescriptorSetLayoutBinding uboLayoutBinding = {};
  uboLayoutBinding.binding                      = U32(bindings.size());
  uboLayoutBinding.descriptorType               = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.descriptorCount              = 1;
  uboLayoutBinding.stageFlags                   = stageFlag;
  uboLayoutBinding.pImmutableSamplers           = nullptr;

  bindings.push_back(uboLayoutBinding);
}

VkDescriptorSetLayout VkCore::CreateDescriptorSetLayout(VkDevice device,
                                                        const std::vector<VkDescriptorSetLayoutBinding>& bindings) {
  VkDescriptorSetLayoutCreateInfo layoutInfo = {};
  layoutInfo.sType                           = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount                    = U32(bindings.size());
  layoutInfo.pBindings                       = bindings.data();

  VkDescriptorSetLayout descriptorSet;
  VERIFY_VK_OP(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSet),
    "Failed to create descriptor set layout");
  return descriptorSet;
}

VkPipelineLayout VkCore::CreatePipelineLayout(VkDevice device,
                                              const std::vector<VkDescriptorSetLayout>& descriptorSets) {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.sType                      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

  // Descriptors Info
  pipelineLayoutInfo.pSetLayouts    = descriptorSets.data();
  pipelineLayoutInfo.setLayoutCount = U32(descriptorSets.size());

  VkPipelineLayout pipelineLayout;
  VERIFY_VK_OP(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout),
    "Failed to create pipeline layout");
  return pipelineLayout;
}

VkShaderModule VkCore::CreateShaderModule(VkDevice device, const std::vector<U8>& code) {
  VkShaderModuleCreateInfo createInfo = {};
  createInfo.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize                 = code.size();
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data()); // byte code ptr conversion

  VkShaderModule shaderModule;
  VERIFY_VK_OP(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule), "Failed to create shader module");
  return shaderModule;
}

std::vector<VkFramebuffer> VkCore::CreateFrameBuffers(VkDevice device,
                                                      VkRenderPass renderPass,
                                                      const VkScopedSwapChain& scopedSwapChain) {
  const auto& swapChainImageViews = scopedSwapChain.m_imageViews;

  std::vector<VkFramebuffer> frameBuffers(swapChainImageViews.size());

  for (SizeType idx = 0; idx < swapChainImageViews.size(); ++idx) {

    const std::array<VkImageView, 1> attachments = {
      swapChainImageViews[idx]
    };

    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass              = renderPass;
    framebufferInfo.attachmentCount         = U32(attachments.size());
    framebufferInfo.pAttachments            = attachments.data();
    framebufferInfo.width                   = scopedSwapChain.m_extent.width;
    framebufferInfo.height                  = scopedSwapChain.m_extent.height;
    framebufferInfo.layers                  = 1;

    VERIFY_VK_OP(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &frameBuffers[idx]),
      "Failed to create framebuffer");
  }

  return frameBuffers;
}

VkCommandPool VkCore::CreateCommandPool(VkDevice device, int queueIndex) {
  VkCommandPoolCreateInfo poolInfo = {};
  poolInfo.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.queueFamilyIndex        = queueIndex;
  poolInfo.flags                   = 0;

  VkCommandPool commandPool;
  VERIFY_VK_OP(vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool), "Failed to create command pool");
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
                                              const std::vector<VkDescriptorPoolSize>& pools,
                                              U32 maxSets) {
  VkDescriptorPoolCreateInfo poolInfo = {};
  poolInfo.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount              = U32(pools.size());
  poolInfo.pPoolSizes                 = pools.data();
  poolInfo.maxSets                    = maxSets;

  VkDescriptorPool descriptorPool;
  VERIFY_VK_OP(vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool), "Failed to create descriptor pool");
  return descriptorPool;
}

VkDescriptorSet VkCore::CreateDescriptorSet(VkDevice device,
                                            VkDescriptorPool descriptorPool,
                                            const std::vector<VkDescriptorSetLayout>& descriptorSets) {
  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType                       = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool              = descriptorPool;
  allocInfo.descriptorSetCount          = U32(descriptorSets.size());
  allocInfo.pSetLayouts                 = descriptorSets.data();

  VkDescriptorSet descriptorSet;
  VERIFY_VK_OP(vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet), "Failed to allocate descriptor set");
  return descriptorSet;
}

VkWriteDescriptorSet VkCore::CreateWriteDescriptorForUniformBuffer(VkDescriptorSet set,
                                                                   U32 layoutIndex,
                                                                   U32 binding,
                                                                   const std::vector<VkDescriptorBufferInfo>&
                                                                   bufferInfos) {
  VkWriteDescriptorSet descriptorWrite = {};
  descriptorWrite.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrite.dstSet               = set;
  descriptorWrite.dstArrayElement      = layoutIndex;
  descriptorWrite.dstBinding           = binding;
  descriptorWrite.descriptorType       = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

  // Attach Buffer Info to WriteDescriptorSet
  descriptorWrite.descriptorCount  = U32(bufferInfos.size());
  descriptorWrite.pBufferInfo      = bufferInfos.data();
  descriptorWrite.pImageInfo       = nullptr;
  descriptorWrite.pTexelBufferView = nullptr;

  return descriptorWrite;
}

void VkCore::UpdateDescriptorSets(VkDevice device, const std::vector<VkWriteDescriptorSet>& descriptorWrites) {
  vkUpdateDescriptorSets(device, U32(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}


VkCommandBuffer VkCore::CreateCommandBuffer(VkDevice device, VkCommandPool commandPool, VkCommandBufferLevel level) {
  VkCommandBuffer commandBuffer;

  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool                 = commandPool;
  allocInfo.level                       = level;
  allocInfo.commandBufferCount          = 1;

  VERIFY_VK_OP(vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer), "Failed to allocate command buffers");

  return commandBuffer;
}


std::vector<VkCommandBuffer> VkCore::CreateCommandBuffers(VkDevice device,
                                                          U32 count,
                                                          VkCommandPool commandPool,
                                                          VkCommandBufferLevel level) {
  std::vector<VkCommandBuffer> commandBuffers(count);
  CreateCommandBuffers(device, count, commandPool, level, commandBuffers);
  return commandBuffers;
}

void VkCore::CreateCommandBuffers(VkDevice device,
                                  U32 count,
                                  VkCommandPool commandPool,
                                  VkCommandBufferLevel level,
                                  std::vector<VkCommandBuffer>& commandBuffers) {

  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool                 = commandPool;
  allocInfo.level                       = level;
  allocInfo.commandBufferCount          = count;

  VERIFY_VK_OP(vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()), "Failed to create command buffers");
}

void VkCore::BeginCommandBuffer(VkCommandBuffer buffer, VkCommandBufferUsageFlags flags) {
  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags                    = flags;
  beginInfo.pInheritanceInfo         = nullptr;

  VERIFY_VK_OP(vkBeginCommandBuffer(buffer, &beginInfo), "Failed to begin recording command buffer");
}

void VkCore::EndCommandBuffer(VkCommandBuffer buffer) {
  VERIFY_VK_OP(vkEndCommandBuffer(buffer), "Failed to begin recording command buffer");
}

VkSemaphore VkCore::CreateSemaphore(VkDevice device) {
  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  VkSemaphore semaphore;
  VERIFY_VK_OP(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore), "Failed to create semaphore");
  return semaphore;
}

void VkCore::CreateSemaphores(VkDevice device, U32 count, std::vector<VkSemaphore>& semaphores) {
  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  for (U32 idx = 0; idx < count; ++idx) {
    VERIFY_VK_OP(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphores[idx]),
      "Failed to create semaphore [multi-create]");
  }
}

std::vector<VkSemaphore> VkCore::CreateSemaphores(VkDevice device, U32 count) {
  std::vector<VkSemaphore> semaphores(count);
  CreateSemaphores(device, count, semaphores);
  return semaphores;
}


VkFence VkCore::CreateFence(VkDevice device, VkFenceCreateFlags flags) {
  VkFenceCreateInfo fenceInfo = {};
  fenceInfo.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags             = flags;

  VkFence fence;
  VERIFY_VK_OP(vkCreateFence(device, &fenceInfo, nullptr, &fence), "Failed to create fence");
  return fence;
}

void VkCore::CreateFences(VkDevice device, U32 count, VkFenceCreateFlags flags, std::vector<VkFence>& fences) {
  VkFenceCreateInfo fenceInfo = {};
  fenceInfo.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags             = flags;

  for (U32 idx = 0; idx < count; ++idx) {
    VERIFY_VK_OP(vkCreateFence(device, &fenceInfo, nullptr, &fences[idx]), "Failed to create fences [multi-create]");
  }
}

std::vector<VkFence> VkCore::CreateFences(VkDevice device, U32 count, VkFenceCreateFlags flags) {
  std::vector<VkFence> fences(count);
  CreateFences(device, count, flags, fences);
  return fences;
}

} // namespace Vulkan
} // namespace Azura
