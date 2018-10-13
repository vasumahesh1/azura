#pragma once
#include "Types.h"
#include "Drawable.h"

namespace Azura {
class Renderer;
}

namespace Azura {
namespace Memory {
class Allocator;
}
}

namespace Azura {
namespace PoolPrimitives {
DrawablePool& AddScreenQuad(Renderer& renderer, U32 renderPass, Memory::Allocator& allocatorTemporary);
}
} // namespace Azura
