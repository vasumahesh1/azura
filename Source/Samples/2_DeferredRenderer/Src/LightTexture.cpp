#include "App/LightTexture.h"

namespace Azura {

LightTexture::LightTexture(U32 numLights, U32 componentsPerLight, Memory::Allocator& allocator)
  : m_buffer(allocator),
    m_numLights(numLights),
    m_pixelsPerLight(U32(std::ceil(componentsPerLight / 4.0f))) {

  m_buffer.Reserve(m_numLights * m_pixelsPerLight);
}
} // namespace Azura
