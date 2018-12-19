
.. _program_listing_file_Source_Azura_RenderSystem_Src_D3D12_D3D12TextureManager.cpp:

Program Listing for File D3D12TextureManager.cpp
================================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Src_D3D12_D3D12TextureManager.cpp>` (``Source\Azura\RenderSystem\Src\D3D12\D3D12TextureManager.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "D3D12/D3D12TextureManager.h"
   
   
   namespace Azura {
   namespace D3D12 {
   
   D3D12TextureManager::D3D12TextureManager(const TextureRequirements& requirements)
     : TextureManager(requirements) {
   }
   } // namespace D3D12
   } // namespace Azura
