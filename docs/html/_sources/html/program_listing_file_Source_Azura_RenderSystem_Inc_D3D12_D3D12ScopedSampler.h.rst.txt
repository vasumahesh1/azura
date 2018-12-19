
.. _program_listing_file_Source_Azura_RenderSystem_Inc_D3D12_D3D12ScopedSampler.h:

Program Listing for File D3D12ScopedSampler.h
=============================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Inc_D3D12_D3D12ScopedSampler.h>` (``Source\Azura\RenderSystem\Inc\D3D12\D3D12ScopedSampler.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "D3D12Core.h"
   
   
   namespace Azura {
   namespace D3D12 {
   
   class D3D12ScopedSampler
   {
   public:
     void Create(const SamplerDesc& desc, const Log& log_D3D12RenderSystem);
     const D3D12_SAMPLER_DESC& GetDesc() const;
   
   private:
     D3D12_SAMPLER_DESC m_desc{};
   };
   
   } // namespace D3D12
   } // namespace Azura
