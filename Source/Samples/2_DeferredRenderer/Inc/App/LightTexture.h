#pragma once
#include "Types.h"
#include <Containers/Vector.h>

#include "App/LightDefinitions.h"

namespace Azura {

class LightTexture {
public:
  LightTexture(U32 numLights, U32 componentsPerLight, Memory::Allocator& allocator);

private:
  Containers::Vector<float> m_buffer;

  U32 m_numLights;
  U32 m_pixelsPerLight;
};

} // namespace Azura
