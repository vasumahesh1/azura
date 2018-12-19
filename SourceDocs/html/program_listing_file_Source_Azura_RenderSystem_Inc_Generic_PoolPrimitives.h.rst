
.. _program_listing_file_Source_Azura_RenderSystem_Inc_Generic_PoolPrimitives.h:

Program Listing for File PoolPrimitives.h
=========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Inc_Generic_PoolPrimitives.h>` (``Source\Azura\RenderSystem\Inc\Generic\PoolPrimitives.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "Types.h"
   #include "Drawable.h"
   
   namespace Azura {
   class Renderer;
   }
   
   namespace Azura {
   namespace Memory {
   class Allocator;
   }
   }
   
   namespace Azura {
   namespace PoolPrimitives {
   DrawablePool& AddScreenQuad(Renderer& renderer, U32 renderPass, Memory::Allocator& allocatorTemporary);
   }
   } // namespace Azura
