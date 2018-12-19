
.. _program_listing_file_Source_Azura_RenderSystem_Test_RenderTestCases.h:

Program Listing for File RenderTestCases.h
==========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Test_RenderTestCases.h>` (``Source\Azura\RenderSystem\Test\RenderTestCases.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "Generic/Renderer.h"
   #include "Math/Core.h"
   
   
   namespace Azura {
   class Log;
   class Window;
   
   struct UniformBufferData {
     Matrix4f m_model;
     Matrix4f m_modelInvTranspose;
     Matrix4f m_viewProj;
   };
   }
   
   namespace RenderTestCases {
     void ExecuteBasicRenderTest(Azura::Renderer& renderer, Azura::Window& window, Azura::U32 renderPass, Azura::U32 uboSlot, const Azura::Log& log_TestCase);
     void ExecuteBasicInstancingTest(Azura::Renderer& renderer, Azura::Window& window, Azura::U32 renderPass, Azura::U32 uboSlot, const Azura::Log& log_TestCase);
     void ExecuteBasicTextureTest(Azura::Renderer& renderer, Azura::Window& window, Azura::U32 renderPass, Azura::U32 uboSlot, Azura::U32 samplerSlot, Azura::U32 samplerTextureSlot, const Azura::Log& log_TestCase);
     void ExecuteBasicDeferredTest(Azura::Renderer & renderer, Azura::Window & window, Azura::U32 renderPass1, Azura::U32 renderPass2, Azura::U32 uboSlot, Azura::U32 samplerSlot, Azura::U32 samplerTextureSlot, const Azura::Log & log_TestCase);
   } // namespace RenderTestCases
