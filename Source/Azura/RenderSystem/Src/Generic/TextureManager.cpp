#include "Generic/TextureManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Azura {

TextureManager::TextureManager(U32 poolSize)
  : m_textureBuffer(poolSize),
    m_textureAllocator(m_textureBuffer, poolSize) {
}

U32 TextureManager::Load(const String& path) {


  // Load failed
  return 0u;
}
} // namespace Azura
