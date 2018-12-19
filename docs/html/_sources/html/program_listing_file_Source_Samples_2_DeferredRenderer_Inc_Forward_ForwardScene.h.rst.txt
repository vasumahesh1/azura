
.. _program_listing_file_Source_Samples_2_DeferredRenderer_Inc_Forward_ForwardScene.h:

Program Listing for File ForwardScene.h
=======================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Samples_2_DeferredRenderer_Inc_Forward_ForwardScene.h>` (``Source\Samples\2_DeferredRenderer\Inc\Forward\ForwardScene.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "App/Scene.h"
   #include "Types.h"
   #include "App/LightTexture.h"
   
   
   namespace Azura {
   
   struct ForwardPass {
     U32 m_passId;
   
     U32 m_uboSlot;
   
     U32 m_sampSlot;
     U32 m_lightSampSlot;
   
     U32 m_texSlot;
     U32 m_normalSlot;
     U32 m_lightTexSlot;
   };
   
   
   class ForwardScene : public Scene {
   
   public:
     explicit ForwardScene(Memory::Allocator& mainAllocator, Memory::Allocator& drawAllocator);
   
     void Initialize(Window& window,
                     const Camera& camera,
                     const MeshObject& sceneMesh,
                     const UniformBufferData& uboData,
                     const SamplerDesc& lightSamplerDesc,
                     const Containers::Vector<PointLight>& lights) override;
     void Update(float timeDelta,
                 const Camera& camera,
                 const UniformBufferData& uboData,
                 const Containers::Vector<PointLight>& lights) override;
     void CleanUp() override;
   
   private:
     ForwardPass m_pass{};
     DrawablePool* m_mainPool{nullptr};
     LightTexture m_lightTexture;
   };
   
   } // namespace Azura
