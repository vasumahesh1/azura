#include "Generic/Renderer.h"

#include <utility>
#include "Generic/Window.h"

namespace Azura {

Renderer::Renderer(ApplicationInfo appInfo,
                   const DeviceRequirements& deviceRequirements,
                   ApplicationRequirements appRequirements,
                   const SwapChainRequirements& swapChainRequirements,
                   const DescriptorRequirements& descriptorRequirements,
                   Memory::Allocator& mainAllocator,
                   Memory::Allocator& drawAllocator,
                   Window& window)
  : m_descriptorSlots(descriptorRequirements.m_descriptorSlots.GetSize(), mainAllocator),
    m_descriptorSetTable(descriptorRequirements.m_descriptorSets.GetSize(), mainAllocator),
    m_mainAllocator(mainAllocator),
    m_drawPoolAllocator(drawAllocator),
    m_window(window),
    m_applicationInfo(std::move(appInfo)),
    m_deviceRequirements(deviceRequirements),
    m_appRequirements(std::move(appRequirements)),
    m_swapChainRequirements(swapChainRequirements),
    m_frameCount(0) {

  U32 setIdx = 0;
  for (const auto& set : descriptorRequirements.m_descriptorSets) {

    U32 bindIdx = 0;
    for(const auto& descriptorId : set)
    {
      const auto& slot = descriptorRequirements.m_descriptorSlots[descriptorId];

      DescriptorSlot setSlot = {};
      setSlot.m_type         = slot.m_type;
      setSlot.m_stages       = slot.m_stages;
      setSlot.m_setIdx       = setIdx;
      setSlot.m_bindIdx      = bindIdx;

      m_descriptorSlots.PushBack(std::move(setSlot));
      ++bindIdx;
    }

    const auto& typeSlot = descriptorRequirements.m_descriptorSlots[set[0]];

    DescriptorTableEntry entry = {};
    entry.m_count = bindIdx;
    entry.m_type = typeSlot.m_type;

    m_descriptorSetTable.PushBack(entry);

    ++setIdx;
  }

  for (const auto& slot : m_descriptorSlots) {
    switch (slot.m_type) {

      case DescriptorType::UniformBuffer:
        ++m_descriptorCount.m_numUniformSlots;
        break;

      case DescriptorType::Sampler:
        ++m_descriptorCount.m_numSamplerSlots;
        break;
      case DescriptorType::SampledImage:
        ++m_descriptorCount.m_numSampledImageSlots;
        break;
      case DescriptorType::CombinedImageSampler:
        ++m_descriptorCount.m_numCombinedSamplerSlots;
        break;

      case DescriptorType::PushConstant:
        ++m_descriptorCount.m_numPushConstantsSlots;
        break;
      default:
        break;
    }
  }
}

void Renderer::EnterRenderFrame() {
}

void Renderer::ExitRenderFrame() {
  m_frameCount = (m_frameCount + 1) % m_swapChainRequirements.m_framesInFlight;
}

const ApplicationInfo& Renderer::GetApplicationInfo() const {
  return m_applicationInfo;
}

const DeviceRequirements& Renderer::GetDeviceRequirements() const {
  return m_deviceRequirements;
}

const ApplicationRequirements& Renderer::GetApplicationRequirements() const {
  return m_appRequirements;
}

const SwapChainRequirements& Renderer::GetSwapchainRequirements() const {
  return m_swapChainRequirements;
}

U32 Renderer::GetCurrentFrame() const {
  return m_frameCount;
}

void Renderer::SetCurrentFrame(U32 frameIdx) {
  m_frameCount = frameIdx;
}
} // namespace Azura
