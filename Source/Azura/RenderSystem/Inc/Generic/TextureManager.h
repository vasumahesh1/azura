#pragma once
#include "Types.h"
#include "Memory/HeapMemoryBuffer.h"
#include "Memory/MonotonicAllocator.h"
#include "Containers/Vector.h"
#include "GenericTypes.h"
#include "Core/RawStorageFormat.h"
#include "Log/Log.h"

namespace Azura {
struct TextureDesc
{
  Bounds3D m_bounds{ 0, 0, 0 };
  U32 m_size{0};
  U32 m_mipLevels{0};
  U32 m_arrayLayers{0};
  ImageType m_type{ImageType::Image2D};
  RawStorageFormat m_format{ RawStorageFormat::R8G8B8A8_UNORM };
};

class TextureManager
{
  struct TextureEntry
  {
    TextureDesc m_desc;
    U32 m_handle;
    Containers::Vector<U8> m_data;

    TextureEntry(Memory::MonotonicAllocator& allocator);
  };

  static U32 s_currentTextureId;

public:
  explicit TextureManager(U32 poolSize, U32 maxTextures);
  U32 Load(const String& path);
  const TextureDesc* GetInfo(U32 handle) const;
  const U8* GetData(U32 handle) const;

private:
  const U32 m_maxTextures;
  Memory::HeapMemoryBuffer m_textureBuffer;
  Memory::MonotonicAllocator m_textureAllocator;
  Containers::Vector<TextureEntry> m_storedTextures;

  Log log_TextureManager;
};
  
} // namespace Azura