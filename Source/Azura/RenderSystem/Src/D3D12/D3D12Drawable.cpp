#include "D3D12/D3D12Drawable.h"


namespace Azura {
namespace D3D12 {

D3D12Drawable::D3D12Drawable(const DrawableCreateInfo& info,
                             U32 numVertexSlots,
                             U32 numInstanceSlots,
                             U32 numUniformSlots,
                             Memory::Allocator& allocator)
  : Drawable(info, numVertexSlots, numInstanceSlots, numUniformSlots, allocator) {
}
} // namespace D3D12
} // namespace Azura