#pragma once
#include "Renderer.h"
#include "Memory/Allocator.h"

namespace Azura {
namespace RenderSystem {
  Renderer* CreateRenderer(Memory::Allocator& allocator);
} // namespace RenderSystem
} // namespace Azura
