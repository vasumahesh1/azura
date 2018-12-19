
.. _program_listing_file_Source_Azura_RenderSystem_Src_D3D12_D3D12ScopedSampler.cpp:

Program Listing for File D3D12ScopedSampler.cpp
===============================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Src_D3D12_D3D12ScopedSampler.cpp>` (``Source\Azura\RenderSystem\Src\D3D12\D3D12ScopedSampler.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "D3D12/D3D12ScopedSampler.h"
   #include "Log/Log.h"
   #include "D3D12/D3D12TypeMapping.h"
   
   
   namespace Azura {
   namespace D3D12 {
   
   void D3D12ScopedSampler::Create(const SamplerDesc& desc, const Log& log_D3D12RenderSystem) {
     const auto textureFilter = ToD3D12_FILTER(desc.m_filter);
     VERIFY_OPT(log_D3D12RenderSystem, textureFilter, "Unknown Texture Filter");
   
     const auto addressU = ToD3D12_TEXTURE_ADDRESS_MODE(desc.m_addressModeU);
     VERIFY_OPT(log_D3D12RenderSystem, addressU, "Unknown Address Mode U");
   
     const auto addressV = ToD3D12_TEXTURE_ADDRESS_MODE(desc.m_addressModeV);
     VERIFY_OPT(log_D3D12RenderSystem, addressU, "Unknown Address Mode V");
   
     const auto addressW = ToD3D12_TEXTURE_ADDRESS_MODE(desc.m_addressModeW);
     VERIFY_OPT(log_D3D12RenderSystem, addressU, "Unknown Address Mode W");
   
     // TODO(vasumahesh1):[SAMPLER]: Expose more options later on
     m_desc.Filter = textureFilter.value();
     m_desc.AddressU = addressU.value();
     m_desc.AddressV = addressV.value();
     m_desc.AddressW = addressW.value();
     m_desc.MipLODBias = 0;
     m_desc.MaxAnisotropy = 16;
     m_desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
     m_desc.MinLOD = 0.0f;
     m_desc.MaxLOD = D3D12_FLOAT32_MAX;
   }
   
   const D3D12_SAMPLER_DESC& D3D12ScopedSampler::GetDesc() const {
     return m_desc;
   }
   } // namespace D3D12
   } // namespace Azura
