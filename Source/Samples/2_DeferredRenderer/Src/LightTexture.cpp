#include "App/LightTexture.h"

namespace Azura {

LightTexture::LightTexture(U32 numLights, U32 componentsPerLight, Memory::Allocator& allocator)
  : m_buffer(allocator),
    m_numLights(numLights),
    m_pixelsPerLight(U32(std::ceil(componentsPerLight / 4.0f))) {

  m_buffer.Resize(m_numLights * m_pixelsPerLight * 4);
}

void LightTexture::Fill(const Containers::Vector<PointLight>& lights) {
  U32 idx = 0;
  for (const auto& light : lights) {
    m_buffer[idx + 0] = light.m_position.x;
    m_buffer[idx + 1] = light.m_position.y;
    m_buffer[idx + 2] = light.m_position.z;
    m_buffer[idx + 3] = light.m_radius;

    m_buffer[idx + 4] = light.m_color.x;
    m_buffer[idx + 5] = light.m_color.y;
    m_buffer[idx + 6] = light.m_color.z;
    m_buffer[idx + 7] = 0;

    idx += 8;
  }
}

const U8* LightTexture::GetBuffer() const {
  return reinterpret_cast<const U8*>(m_buffer.Data()); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
}

U32 LightTexture::GetSize() const {
  return m_buffer.GetMaxSize() * sizeof(float);
}

TextureDesc LightTexture::GetTextureDesc() const {
  TextureDesc desc = {};

  desc.m_format = RawStorageFormat::R32G32B32A32_FLOAT;
  desc.m_arrayLayers = 1;
  desc.m_mipLevels = 1;
  desc.m_bounds.m_width = m_numLights;
  desc.m_bounds.m_height = m_pixelsPerLight;
  desc.m_bounds.m_depth = 1;
  desc.m_type = ImageType::Image2D;
  desc.m_size = GetSize();

  return desc;
}
} // namespace Azura
