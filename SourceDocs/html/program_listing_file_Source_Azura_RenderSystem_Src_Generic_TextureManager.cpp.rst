
.. _program_listing_file_Source_Azura_RenderSystem_Src_Generic_TextureManager.cpp:

Program Listing for File TextureManager.cpp
===========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Src_Generic_TextureManager.cpp>` (``Source\Azura\RenderSystem\Src\Generic\TextureManager.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Generic/TextureManager.h"
   
   #define STB_IMAGE_IMPLEMENTATION
   #include "stb_image.h"
   
   namespace Azura {
   
   U32 TextureManager::s_currentTextureId = 0;
   
   TextureManager::TextureEntry::TextureEntry(Memory::MonotonicAllocator& allocator)
     : m_handle(0),
       m_data(allocator) {
   }
   
   TextureManager::TextureManager(const TextureRequirements& requirements)
     : log_TextureManager(Log("TextureManager")),
       m_maxTextures(requirements.m_maxCount),
       m_textureBuffer(requirements.m_poolSize + requirements.m_maxCount * sizeof(TextureEntry)),
       m_textureAllocator(m_textureBuffer, requirements.m_poolSize + requirements.m_maxCount * sizeof(TextureEntry)),
       m_storedTextures(requirements.m_maxCount, m_textureAllocator) {
     stbi_set_flip_vertically_on_load(true); // NOLINT
     s_currentTextureId = 0; // Reset
   }
   
   U32 TextureManager::Load(const String& path) {
     if (s_currentTextureId == m_maxTextures) {
       // Max Texture Limit Reached
       LOG_ERR(log_TextureManager, LOG_LEVEL, "Max Texture Limit Reached (%d / %d) Increase the Max Textures to Load",
         s_currentTextureId, m_maxTextures);
       return 0u;
     }
   
     int width    = 0;
     int height   = 0;
     int channels = 0;
   
     const auto imagePixels = stbi_load(path.c_str(), &width, &height, &channels, 4);
   
     if (imagePixels == nullptr) {
       // Load failed
       LOG_ERR(log_TextureManager, LOG_LEVEL, "Failed to load texture from STB");
       return 0u;
     }
   
     s_currentTextureId++;
   
     const U32 bytePerPixel = 4;
   
     // TODO(vasumahesh1):[TEXTURE]: Support Mip Maps
     TextureEntry entry    = {m_textureAllocator};
     entry.m_desc.m_bounds.m_width  = width;
     entry.m_desc.m_bounds.m_height = height;
     entry.m_desc.m_size   = bytePerPixel * width * height;
     entry.m_handle        = s_currentTextureId;
     entry.m_data.Resize(entry.m_desc.m_size);
   
     // TODO(vasumahesh1):[TEXTURE]: Support 3D Textures
     if (height == 1) {
       entry.m_desc.m_type = ImageType::Image1D;
     }
   
     // Copy Image Data to our Vector
     std::memcpy(entry.m_data.Data(), imagePixels, entry.m_desc.m_size);
   
     LOG_DBG(log_TextureManager, LOG_LEVEL, "Created Texture: %s >> Size: %d bytes >> Dim: %d px x %d px", path.c_str(),
       entry.m_desc.m_size, entry.m_desc.m_bounds.m_width, entry.m_desc.m_bounds.m_height);
   
     m_storedTextures.PushBack(std::move(entry));
   
     stbi_image_free(imagePixels);
   
     return s_currentTextureId;
   }
   
   const TextureDesc* TextureManager::GetInfo(U32 handle) const {
   
     for (const auto& textureEntry : m_storedTextures) {
       if (textureEntry.m_handle == handle) {
         return &textureEntry.m_desc;
       }
     }
   
     LOG_ERR(log_TextureManager, LOG_LEVEL, "Cant Find Texture Handle: %d", handle);
     return nullptr;
   }
   
   const U8* TextureManager::GetData(U32 handle) const {
     for (const auto& textureEntry : m_storedTextures) {
       if (textureEntry.m_handle == handle) {
         return textureEntry.m_data.Data();
       }
     }
   
     LOG_ERR(log_TextureManager, LOG_LEVEL, "Cant Find Texture Data for Handle: %d", handle);
     return nullptr;
   }
   } // namespace Azura
