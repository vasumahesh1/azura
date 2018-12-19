
.. _program_listing_file_Source_Samples_2_DeferredRenderer_Inc_App_LightTexture.h:

Program Listing for File LightTexture.h
=======================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Samples_2_DeferredRenderer_Inc_App_LightTexture.h>` (``Source\Samples\2_DeferredRenderer\Inc\App\LightTexture.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "Types.h"
   #include <Containers/Vector.h>
   
   #include "App/Common.h"
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
