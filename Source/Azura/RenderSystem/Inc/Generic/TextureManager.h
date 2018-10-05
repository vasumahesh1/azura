#pragma once
#include "Types.h"
#include "Memory/HeapMemoryBuffer.h"
#include "Memory/MonotonicAllocator.h"

namespace Azura {

class TextureManager
{
public:
  explicit TextureManager(U32 poolSize);
  U32 Load(const String& path);

private:
  Memory::HeapMemoryBuffer m_textureBuffer;
  Memory::MonotonicAllocator m_textureAllocator;
};
  
} // namespace Azura