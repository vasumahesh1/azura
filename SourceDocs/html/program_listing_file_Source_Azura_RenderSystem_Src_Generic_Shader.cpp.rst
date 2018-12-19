
.. _program_listing_file_Source_Azura_RenderSystem_Src_Generic_Shader.cpp:

Program Listing for File Shader.cpp
===================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Src_Generic_Shader.cpp>` (``Source\Azura\RenderSystem\Src\Generic\Shader.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Generic/Shader.h"
   
   namespace Azura {
   
   Shader::Shader(const String& filePath, const String& extension)
     : m_filePath(filePath + "." + extension),
       m_stage(),
       m_entryPoint("main") {
   }
   
   void Shader::SetStage(const ShaderStage stage) {
     m_stage = stage;
   }
   
   void Shader::SetEntryPoint(const String& entryPoint) {
     m_entryPoint = entryPoint;
   }
   
   const String& Shader::GetFilePath() const {
     return m_filePath;
   }
   
   const String& Shader::GetShaderEntryPoint() const {
     return m_entryPoint;
   }
   
   ShaderStage Shader::GetShaderStage() const {
     return m_stage;
   }
   
   } // namespace Azura
