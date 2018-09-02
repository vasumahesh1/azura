#pragma once
#include "Memory/Allocator.h"
#include "Renderer.h"

namespace Azura {
class Window;
}

namespace Azura {
namespace RenderSystem {

// TODO(vasumahesh1):[PERF]: On Random Heap.

std::unique_ptr<Renderer> CreateRenderer(const ApplicationInfo& appInfo,
                                         const DeviceRequirements& deviceRequirements,
                                         const SwapChainRequirement& swapChainRequirement,
                                         Memory::Allocator& mainAllocator,
                                         Memory::Allocator& drawAllocator,
                                         Window& window);

std::unique_ptr<Window> CreateWindow(String title, U32 width, U32 height);

}  // namespace RenderSystem
}  // namespace Azura
