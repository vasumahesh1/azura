#include "Vulkan/VkScopedSwapChain.h"
#include "Vulkan/VkTypes.h"
#include "Vulkan/VkCore.h"
#include "Generic/Renderer.h"
#include "Vulkan/VkTypeMapping.h"
#include "Vulkan/VkMacros.h"
#include "Memory/MemoryFactory.h"

namespace Azura {
namespace Vulkan {

namespace {
VkPresentModeKHR ChooseSwapPresentMode(const Containers::Vector<VkPresentModeKHR>& availablePresentModes) {
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

VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const Containers::Vector<VkSurfaceFormatKHR>& availableFormats,
                                           const SwapChainRequirements& requirement,
                                           const Log& log_VulkanRenderSystem) {
  const auto format = ToVkFormat(requirement.m_format);
  VERIFY_OPT(log_VulkanRenderSystem, format, "Unknown Format");

  const auto colorSpace = ToVkColorSpaceKHR(requirement.m_colorSpace);
  VERIFY_OPT(log_VulkanRenderSystem, colorSpace, "Unknown Colorspace");

  LOG_DBG(log_VulkanRenderSystem, LOG_LEVEL, "Checking for %d availableFormats", availableFormats.GetSize());

  // no preferred format
  if (availableFormats.GetSize() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
    return {format.value(), colorSpace.value()};
  }

  for (const auto& availableFormat : availableFormats) {
    if (availableFormat.format == format.value() && availableFormat.colorSpace == colorSpace.value()) {
      return availableFormat;
    }
  }

  throw std::runtime_error("Cannot get proper required swapchain format");
}

VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const SwapChainRequirements& requirement) {
  if (capabilities.currentExtent.width != std::numeric_limits<U32>::max()) {
    return capabilities.currentExtent;
  }

  VkExtent2D actualExtent = {requirement.m_extent.m_width, requirement.m_extent.m_height};
  actualExtent.width      =
    std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
  actualExtent.height =
    std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
  return actualExtent;
}

} // namespace

VkScopedSwapChain::VkScopedSwapChain(Memory::Allocator& allocator, Log logger)
  : log_VulkanRenderSystem(std::move(logger)),
    m_swapChain(),
    m_extent(),
    m_surfaceFormat(),
    m_images(allocator),
    m_depthImage(log_VulkanRenderSystem) {
}

void VkScopedSwapChain::Create(VkDevice device,
                               VkPhysicalDevice physicalDevice,
                               VkQueue graphicsQueue,
                               VkCommandPool graphicsCommandPool,
                               VkSurfaceKHR surface,
                               const VkQueueIndices& queueIndices,
                               const SwapChainDeviceSupport& swapChainSupport,
                               const SwapChainRequirements& swapChainRequirement,
                               const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties) {

  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 1024);

  const VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.m_presentModes);
  m_surfaceFormat                    = ChooseSwapSurfaceFormat(swapChainSupport.m_formats, swapChainRequirement,
                                                               log_VulkanRenderSystem);
  m_extent = ChooseSwapExtent(swapChainSupport.m_capabilities, swapChainRequirement);

  // TODO(vasumahesh1): Need requirement?
  // Set Queue Length of SwapChain
  m_imageCount = swapChainSupport.m_capabilities.minImageCount + 1;

  if (swapChainSupport.m_capabilities.maxImageCount > 0
      && m_imageCount > swapChainSupport.m_capabilities.maxImageCount) {
    m_imageCount = swapChainSupport.m_capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo = {};
  createInfo.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface                  = surface;
  createInfo.minImageCount            = m_imageCount;
  createInfo.imageFormat              = m_surfaceFormat.format;
  createInfo.imageColorSpace          = m_surfaceFormat.colorSpace;
  createInfo.imageExtent              = m_extent;
  createInfo.imageArrayLayers         = 1;
  createInfo.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

  // TODO(vasumahesh1): Possible bug if Graphics Queue is not the same as Present Queue. How to handle?
  if (queueIndices.m_transferFamily != -1 && queueIndices.m_isTransferQueueRequired) {
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

  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_swapChain),
    "Failed to create swap chain");

  vkGetSwapchainImagesKHR(device, m_swapChain, &m_imageCount, nullptr);
  m_images.Reserve(m_imageCount);

  Containers::Vector<VkImage> tempImages{Containers::ContainerExtent{m_imageCount}, allocatorTemporary};
  vkGetSwapchainImagesKHR(device, m_swapChain, &m_imageCount, tempImages.Data());

  TextureDesc swapChainDesc       = {};
  swapChainDesc.m_arrayLayers     = 1;
  swapChainDesc.m_mipLevels       = 1;
  swapChainDesc.m_bounds.m_width  = m_extent.width;
  swapChainDesc.m_bounds.m_height = m_extent.height;
  swapChainDesc.m_bounds.m_depth  = 1;
  swapChainDesc.m_format          = swapChainRequirement.m_format;
  swapChainDesc.m_type            = ImageType::Image2D;

  for (U32 idx       = 0; idx < m_imageCount; ++idx) {
    auto scopedImage = VkScopedImage(device, swapChainDesc, tempImages[idx], log_VulkanRenderSystem);
    scopedImage.CreateImageView(ImageViewType::ImageView2D);
    m_images.PushBack(scopedImage);
  }

  // Check if Depth Format is supported by Device or not.
  const bool depthFormatCheck = VkCore::QueryFormatFeatureSupport(
                                                                  physicalDevice,
                                                                  VkCore::GetVkFormat(swapChainRequirement.
                                                                                      m_depthFormat,
                                                                                      log_VulkanRenderSystem),
                                                                  [](const VkFormatProperties& prop) -> bool
                                                                  {
                                                                    return (prop.optimalTilingFeatures &
                                                                            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
                                                                           ) ==
                                                                           VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
  });

  VERIFY_TRUE(log_VulkanRenderSystem, depthFormatCheck, "Depth Format Not Supported");

  TextureDesc depthDesc = swapChainDesc;
  depthDesc.m_format = swapChainRequirement.m_depthFormat;
  depthDesc.m_arrayLayers     = 1;
  depthDesc.m_mipLevels       = 1;
  depthDesc.m_size = GetFormatSize(swapChainRequirement.m_depthFormat) * swapChainDesc.m_bounds.m_width * swapChainDesc.m_bounds.m_height;

  m_depthImage.Create(device, depthDesc, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, physicalDeviceMemoryProperties);
  m_depthImage.CreateImageView(ImageViewType::ImageView2D);

  VkCommandBuffer depthCommandBuffer = VkCore::CreateCommandBuffer(device, graphicsCommandPool,
    VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    log_VulkanRenderSystem);
  VkCore::BeginCommandBuffer(depthCommandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, log_VulkanRenderSystem);


  const ImageTransition layoutUndefined{
    VK_IMAGE_LAYOUT_UNDEFINED, 0, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
  };

  const ImageTransition layoutDepth{
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
  };

  m_depthImage.TransitionLayout(depthCommandBuffer, layoutUndefined, layoutDepth);

  // Flush & Wait
  VkCore::FlushCommandBuffer(device, depthCommandBuffer, graphicsQueue, log_VulkanRenderSystem);
  vkFreeCommandBuffers(device, graphicsCommandPool, 1, &depthCommandBuffer);
}

void VkScopedSwapChain::CleanUp(VkDevice device) {
  // Swapchains have a different cleanup compared to VkScopedImage
  for (const auto& image : m_images) {
    vkDestroyImageView(device, image.View(), nullptr);
  }

  m_depthImage.CleanUp();

  vkDestroySwapchainKHR(device, m_swapChain, nullptr);
}

VkSwapchainKHR VkScopedSwapChain::Real() const {
  return m_swapChain;
}

VkFormat VkScopedSwapChain::GetSurfaceFormat() const {
  return m_surfaceFormat.format;
}

VkFormat VkScopedSwapChain::GetDepthFormat() const {
  return m_depthImage.GetRealFormat();
}

const VkExtent2D& VkScopedSwapChain::GetExtent() const {
  return m_extent;
}

const VkScopedImage& VkScopedSwapChain::GetImage(int frameIdx) const {
  return m_images[frameIdx];
}

const VkScopedImage& VkScopedSwapChain::GetDepthImage() const {
  return m_depthImage;
}

const Containers::Vector<VkScopedImage>& VkScopedSwapChain::GetAllImages() const {
  return m_images;
}
} // namespace Vulkan
} // namespace Azura
