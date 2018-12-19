
.. _program_listing_file_Source_Azura_RenderSystem_Test_Renderer_test.h:

Program Listing for File Renderer_test.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Test_Renderer_test.h>` (``Source\Azura\RenderSystem\Test\Renderer_test.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "Memory/HeapMemoryBuffer.h"
   #include "Memory/MonotonicAllocator.h"
   #include "gtest/gtest.h"
   #include "Memory/RangeAllocator.h"
   #include "Log/Log.h"
   
   using namespace Azura::Memory;
   
   class RendererTest : public ::testing::Test {
   protected:
     RendererTest()
       : log_TestRenderer(Azura::Log("TestRenderer"))
     {}
   
     virtual ~RendererTest() = default;
   
     Azura::Log log_TestRenderer;
   };
