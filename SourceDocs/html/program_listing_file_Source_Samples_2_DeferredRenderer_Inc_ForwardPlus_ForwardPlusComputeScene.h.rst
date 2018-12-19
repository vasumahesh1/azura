
.. _program_listing_file_Source_Samples_2_DeferredRenderer_Inc_ForwardPlus_ForwardPlusComputeScene.h:

Program Listing for File ForwardPlusComputeScene.h
==================================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Samples_2_DeferredRenderer_Inc_ForwardPlus_ForwardPlusComputeScene.h>` (``Source\Samples\2_DeferredRenderer\Inc\ForwardPlus\ForwardPlusComputeScene.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "App/Scene.h"
   #include "Types.h"
   #include "App/LightTexture.h"
   
   
   namespace Azura {
   
   struct ForwardPlusComputePass {
     U32 m_gBufferPassId;
     U32 m_computePassId;
     U32 m_shadingPassId;
   
     U32 m_uboSlot;
     U32 m_computeUBO;
   
     U32 m_sampSlot;
     U32 m_lightSampSlot;
   
     U32 m_texSlot;
     U32 m_normalSlot;
     };
   
   
   class ForwardPlusComputeScene : public Scene {
   
   public:
     explicit ForwardPlusComputeScene(Memory::Allocator& mainAllocator, Memory::Allocator& drawAllocator);
   
     void Initialize(Window& window,
       const Camera& camera,
                     const MeshObject& sceneMesh,
                     const UniformBufferData& uboData,
                     const SamplerDesc& lightSamplerDesc,
                     const Containers::Vector<PointLight>& lights) override;
     void Update(float timeDelta, const Camera& camera, const UniformBufferData& uboData, const Containers::Vector<PointLight>& lights) override;
     void CleanUp() override;
   
   private:
     void FillFrustums(const Window& window, const Camera& camera);
   
     ForwardPlusComputePass m_pass{};
     DrawablePool* m_mainPool{nullptr};
     ComputePool* m_computePool{nullptr};
     LightTexture m_lightTexture;
     LightUBO m_lightUBO;
   
     Containers::Vector<Frustum> m_frustums;
   
   };
   
   } // namespace Azura
