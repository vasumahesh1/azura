#pragma once
#include "Types.h"
#include <Containers/Vector.h>

#include "App/LightDefinitions.h"
#include "D3D12/D3D12TextureManager.h"

namespace Azura {

class LightTexture {
public:
  LightTexture(U32 numLights, U32 componentsPerLight, Memory::Allocator& allocator);

  void Fill(const Containers::Vector<PointLight>& lights);

  const U8* GetBuffer() const;

  U32 GetSize() const;

  TextureDesc GetTextureDesc() const;

private:
  Containers::Vector<float> m_buffer;

  U32 m_numLights;
  U32 m_pixelsPerLight;
};

} // namespace Azura
