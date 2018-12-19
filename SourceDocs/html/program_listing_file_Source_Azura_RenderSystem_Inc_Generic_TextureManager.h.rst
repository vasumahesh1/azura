
.. _program_listing_file_Source_Azura_RenderSystem_Inc_Generic_TextureManager.h:

Program Listing for File TextureManager.h
=========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Inc_Generic_TextureManager.h>` (``Source\Azura\RenderSystem\Inc\Generic\TextureManager.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "Types.h"
   #include "Memory/HeapMemoryBuffer.h"
   #include "Memory/MonotonicAllocator.h"
   #include "Containers/Vector.h"
   #include "GenericTypes.h"
   #include "Log/Log.h"
   
   namespace Azura {
   struct TextureRequirements;
   
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
     explicit TextureManager(const TextureRequirements& requirements);
     U32 Load(const String& path);
     const TextureDesc* GetInfo(U32 handle) const;
     const U8* GetData(U32 handle) const;
   
     Log log_TextureManager;
   
   private:
     const U32 m_maxTextures;
     Memory::HeapMemoryBuffer m_textureBuffer;
     Memory::MonotonicAllocator m_textureAllocator;
     Containers::Vector<TextureEntry> m_storedTextures;
   };
     
   } // namespace Azura
