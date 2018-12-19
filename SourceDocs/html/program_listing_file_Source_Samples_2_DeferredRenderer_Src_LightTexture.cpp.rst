
.. _program_listing_file_Source_Samples_2_DeferredRenderer_Src_LightTexture.cpp:

Program Listing for File LightTexture.cpp
=========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Samples_2_DeferredRenderer_Src_LightTexture.cpp>` (``Source\Samples\2_DeferredRenderer\Src\LightTexture.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

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
   
     const U32 rowWidth = m_numLights * 4;
   
     for (const auto& light : lights) {
       m_buffer[idx + 0] = light.m_position[0];
       m_buffer[idx + 1] = light.m_position[1];
       m_buffer[idx + 2] = light.m_position[2];
       m_buffer[idx + 3] = light.m_radius;
   
       m_buffer[rowWidth + idx + 0] = light.m_color[0];
       m_buffer[rowWidth + idx + 1] = light.m_color[1];
       m_buffer[rowWidth + idx + 2] = light.m_color[2];
       m_buffer[rowWidth + idx + 3] = 1.0;
   
       idx += 4;
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
