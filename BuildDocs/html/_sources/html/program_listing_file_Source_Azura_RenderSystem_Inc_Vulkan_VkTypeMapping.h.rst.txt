
.. _program_listing_file_Source_Azura_RenderSystem_Inc_Vulkan_VkTypeMapping.h:

Program Listing for File VkTypeMapping.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Inc_Vulkan_VkTypeMapping.h>` (``Source\Azura\RenderSystem\Inc\Vulkan\VkTypeMapping.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <vulkan/vulkan_core.h>
   #include <optional>
   #include "Generic/GenericTypes.h"
   #include "Core/RawStorageFormat.h"
   
   namespace Azura {
   namespace Vulkan {
   namespace Impl {
   
   #define CREATE_MAPPER_H(FROM_FORMAT, TO_FORMAT) std::optional<TO_FORMAT> To##TO_FORMAT(FROM_FORMAT inputFormat)
   
   } // namespace Impl
   
   // TODO(vasumahesh1): Enable enum mappings when all mappings are complete to prevent -Wswitch issues
   
   CREATE_MAPPER_H(RawStorageFormat, VkFormat);
   // CREATE_MAPPER_H(VkFormat, RawStorageFormat);
   
   CREATE_MAPPER_H(RawStorageFormat, VkIndexType);
   // CREATE_MAPPER_H(VkIndexType, RawStorageFormat);
   
   CREATE_MAPPER_H(ColorSpace, VkColorSpaceKHR);
   // CREATE_MAPPER_H(VkColorSpaceKHR, ColorSpace);
   
   CREATE_MAPPER_H(PresentModes, VkPresentModeKHR);
   // CREATE_MAPPER_H(VkPresentModeKHR, PresentModes);
   
   CREATE_MAPPER_H(ShaderStage, VkShaderStageFlagBits);
   // CREATE_MAPPER_H(VkShaderStageFlagBits, ShaderStage);
   
   CREATE_MAPPER_H(PrimitiveTopology, VkPrimitiveTopology);
   // CREATE_MAPPER_H(VkPrimitiveTopology, PrimitiveTopology);
   
   CREATE_MAPPER_H(CullMode, VkCullModeFlags);
   // CREATE_MAPPER_H(VkCullModeFlags, CullMode);
   
   CREATE_MAPPER_H(FrontFace, VkFrontFace);
   // CREATE_MAPPER_H(VkFrontFace, FrontFace);
   
   CREATE_MAPPER_H(BufferUsageRate, VkVertexInputRate);
   
   CREATE_MAPPER_H(ImageType, VkImageType);
   
   CREATE_MAPPER_H(ImageViewType, VkImageViewType);
   // CREATE_MAPPER_H(VkImageType, ImageType);
   
   CREATE_MAPPER_H(BlendFactor, VkBlendFactor);
   // CREATE_MAPPER_H(VkBlendFactor, BlendFactor);
   
   CREATE_MAPPER_H(BlendOp, VkBlendOp);
   // CREATE_MAPPER_H(VkBlendOp, BlendOp);
   
   VkShaderStageFlagBits GetCombinedShaderStageFlag(ShaderStage stage);
   
   } // namespace Vulkan
   } // namespace Azura
