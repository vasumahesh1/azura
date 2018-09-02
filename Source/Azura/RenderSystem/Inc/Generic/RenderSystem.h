#pragma once
#include "Memory/Allocator.h"
#include "Renderer.h"

namespace Azura {
namespace RenderSystem {
Renderer* CreateRenderer(Memory::Allocator& allocator);
}  // namespace RenderSystem
}  // namespace Azura
