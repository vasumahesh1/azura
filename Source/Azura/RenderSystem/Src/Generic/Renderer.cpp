#include "Generic/Renderer.h"
#include <utility>
#include "Utils/Macros.h"

namespace Azura {

Renderer::Renderer(ApplicationInfo appInfo, const DeviceRequirements& deviceRequirements, Memory::Allocator& allocator)
    : m_applicationInfo(std::move(appInfo)), m_deviceRequirements(deviceRequirements), m_allocator(allocator) {}

const ApplicationInfo& Renderer::GetApplicationInfo() const {
  return m_applicationInfo;
}

const DeviceRequirements& Renderer::GetDeviceRequirements() const {
  return m_deviceRequirements;
}

Memory::Allocator& Renderer::GetAllocator() const {
  return m_allocator;
}
}  // namespace Azura
