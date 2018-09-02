#include "Vulkan/VkTypes.h"

namespace Azura {
namespace Vulkan {

bool VkQueueIndices::IsComplete() const {
  const bool basicRequirement = m_graphicsFamily >= 0 && m_presentFamily >= 0;

  if (m_isTransferQueueRequired) {
    return basicRequirement && m_transferFamily >= 0;
  }

  return basicRequirement;
}

std::array<U32, 2> VkQueueIndices::GetIndicesArray() const {
  return {uint32_t(m_graphicsFamily), uint32_t(m_transferFamily)};
}

U32 VkQueueIndices::GetActiveSize() const {
  return m_isTransferQueueRequired ? 2 : 1;
}

SwapChainDeviceSupport::SwapChainDeviceSupport(Memory::Allocator& allocator)
    : m_formats(allocator), m_presentModes(allocator) {}

bool SwapChainDeviceSupport::IsSupported() const {
  return !m_formats.IsEmpty() && !m_presentModes.IsEmpty();
}

VkScopedSwapChain::VkScopedSwapChain(Memory::Allocator& allocator)
    : m_swapChain(), m_extent(), m_surfaceFormat(), m_images(allocator), m_imageViews(allocator) {}

}  // namespace Vulkan
}  // namespace Azura
