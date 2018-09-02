#include "Generic/RenderSystem.h"
#include "Utils/Macros.h"

namespace Azura {
Renderer* RenderSystem::CreateRenderer(Memory::Allocator& allocator) {
  UNUSED(allocator);
  return nullptr;
}
}  // namespace Azura
