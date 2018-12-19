
.. _program_listing_file_Source_Azura_RenderSystem_Inc_D3D12_D3D12TextureManager.h:

Program Listing for File D3D12TextureManager.h
==============================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Inc_D3D12_D3D12TextureManager.h>` (``Source\Azura\RenderSystem\Inc\D3D12\D3D12TextureManager.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "Generic/TextureManager.h"
   
   
   namespace Azura {
   namespace D3D12 {
   
   class D3D12TextureManager : public TextureManager
   {
   
   public:
     explicit D3D12TextureManager(const TextureRequirements& requirements);
   };
   
   } // namespace D3D12
   } // namespace Azura
