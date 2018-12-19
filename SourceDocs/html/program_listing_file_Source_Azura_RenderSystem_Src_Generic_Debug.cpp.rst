
.. _program_listing_file_Source_Azura_RenderSystem_Src_Generic_Debug.cpp:

Program Listing for File Debug.cpp
==================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Src_Generic_Debug.cpp>` (``Source\Azura\RenderSystem\Src\Generic\Debug.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Generic/Debug.h"
   
   namespace Azura {
   
   const char* DescriptorTypeToString(DescriptorType type) {
     switch (type) {
       CASE_STR(DescriptorType::UniformBuffer);
       CASE_STR(DescriptorType::Sampler);
       CASE_STR(DescriptorType::SampledImage);
       CASE_STR(DescriptorType::CombinedImageSampler);
       CASE_STR(DescriptorType::PushConstant);
       CASE_STR(DescriptorType::UnorderedView);
   
       default:
         return "Unknown Descriptor Type";
     }
   }
   
   } // namespace Azura
