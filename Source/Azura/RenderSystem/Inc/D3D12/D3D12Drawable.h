#pragma once
#include "Generic/Drawable.h"


namespace Azura {
namespace D3D12 {

class D3D12Drawable : public Drawable
{

public:
  D3D12Drawable(const DrawableCreateInfo& info,
                U32 numVertexSlots,
                U32 numInstanceSlots,
                U32 numUniformSlots,
                Memory::Allocator& allocator);
};

} // namespace D3D12
} // namespace Azura