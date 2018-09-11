#include "Generic/Renderer.h"
#include <utility>
#include "Utils/Macros.h"

namespace Azura {

Renderer::Renderer(ApplicationInfo appInfo,
                   const DeviceRequirements& deviceRequirements,
                   ApplicationRequirements appRequirements,
                   const SwapChainRequirements& swapChainRequirements,
                   Memory::Allocator& allocator)
  : m_applicationInfo(std::move(appInfo)),
    m_deviceRequirements(deviceRequirements),
    m_appRequirements(std::move(appRequirements)),
    m_swapChainRequirements(swapChainRequirements),
    m_allocator(allocator),
    m_frameCount(0) {
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

Memory::Allocator& Renderer::GetAllocator() const {
  return m_allocator;
}

U32 Renderer::GetCurrentFrame() const {
  return m_frameCount;
}
} // namespace Azura
