
.. _program_listing_file_Source_Azura_RenderSystem_Inc_D3D12_D3D12TypeMapping.h:

Program Listing for File D3D12TypeMapping.h
===========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Inc_D3D12_D3D12TypeMapping.h>` (``Source\Azura\RenderSystem\Inc\D3D12\D3D12TypeMapping.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <optional>
   #include "Generic/GenericTypes.h"
   #include "Core/RawStorageFormat.h"
   #include "D3D12Core.h"
   
   namespace Azura {
   namespace D3D12 {
   namespace Impl
   {
   #define CREATE_MAPPER_H(FROM_FORMAT, TO_FORMAT) std::optional<TO_FORMAT> To##TO_FORMAT(FROM_FORMAT inputFormat)
   } // namespace Impl
   
   CREATE_MAPPER_H(RawStorageFormat, DXGI_FORMAT);
   // CREATE_MAPPER_H(DXGI_FORMAT, RawStorageFormat);
   
   CREATE_MAPPER_H(BufferUsageRate, D3D12_INPUT_CLASSIFICATION);
   CREATE_MAPPER_H(ImageType, D3D12_RESOURCE_DIMENSION);
   CREATE_MAPPER_H(ImageViewType, D3D12_SRV_DIMENSION);
   CREATE_MAPPER_H(ImageViewType, D3D12_UAV_DIMENSION);
   CREATE_MAPPER_H(ImageViewType, D3D12_DSV_DIMENSION);
   CREATE_MAPPER_H(ImageViewType, D3D12_RTV_DIMENSION);
   CREATE_MAPPER_H(CullMode, D3D12_CULL_MODE);
   CREATE_MAPPER_H(TextureAddressMode, D3D12_TEXTURE_ADDRESS_MODE);
   CREATE_MAPPER_H(TextureFilter, D3D12_FILTER);
   CREATE_MAPPER_H(BlendOp, D3D12_BLEND_OP);
   CREATE_MAPPER_H(BlendFactor, D3D12_BLEND);
   
   } // namespace D3D12
   } // namespace Azura
