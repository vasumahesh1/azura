#include "Vulkan/VkScopedSampler.h"
#include "Vulkan/VkMacros.h"

namespace Azura {
namespace Vulkan {

VkScopedSampler::VkScopedSampler(VkDevice device, Log logger)
  : m_device(device),
    log_VulkanRenderSystem(std::move(logger)) {

  // TODO(vasumahesh1):[SAMPLER]: Generalize this!
  VkSamplerCreateInfo samplerInfo     = {};
  samplerInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter               = VK_FILTER_NEAREST;
  samplerInfo.minFilter               = VK_FILTER_NEAREST;
  samplerInfo.addressModeU            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  samplerInfo.addressModeV            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  samplerInfo.addressModeW            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  samplerInfo.anisotropyEnable        = VK_FALSE;
  samplerInfo.maxAnisotropy           = 1;
  samplerInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE; // [0,1) addressing
  samplerInfo.compareEnable           = VK_FALSE;
  samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_NEAREST;
  samplerInfo.mipLodBias              = 0.0f;
  samplerInfo.minLod                  = 0.0f;
  samplerInfo.maxLod                  = 0.0f; // Force use 0th Mip Map for Rendering.

  VERIFY_VK_OP(log_VulkanRenderSystem, vkCreateSampler(m_device, &samplerInfo, nullptr, &m_sampler),
    "Failed to create texture sampler");
}

VkSampler VkScopedSampler::Real() const {
  return m_sampler;
}

void VkScopedSampler::CleanUp() const {
  vkDestroySampler(m_device, m_sampler, nullptr);
}

} // namespace Vulkan
} // namespace Azura
