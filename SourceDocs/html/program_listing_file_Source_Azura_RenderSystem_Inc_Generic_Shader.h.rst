
.. _program_listing_file_Source_Azura_RenderSystem_Inc_Generic_Shader.h:

Program Listing for File Shader.h
=================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Inc_Generic_Shader.h>` (``Source\Azura\RenderSystem\Inc\Generic\Shader.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "GenericTypes.h"
   
   namespace Azura {
   
   class Shader {
   public:
     Shader(const String& filePath, const String& extension);
     void SetStage(ShaderStage stage);
     void SetEntryPoint(const String& entryPoint);
   
     ShaderStage GetShaderStage() const;
   
   protected:
     const String& GetFilePath() const;
     const String& GetShaderEntryPoint() const;
   
   private:
     const String m_filePath;
     ShaderStage m_stage;
     String m_entryPoint;
   };
   } // namespace Azura
