
.. _program_listing_file_Source_AssetManager_Inc_AssetManager_GLTFLoader.h:

Program Listing for File GLTFLoader.h
=====================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_AssetManager_Inc_AssetManager_GLTFLoader.h>` (``Source\AssetManager\Inc\AssetManager\GLTFLoader.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "Types.h"
   #include <GLTFSDK/Document.h>
   #include <GLTFSDK/GLTFResourceReader.h>
   
   namespace Azura {
   class Log;
   enum class AssetLocation;
   
   class GLTFMeshInterface
   {
   public:
     GLTFMeshInterface(const String& manifest, std::unique_ptr<const Microsoft::glTF::GLTFResourceReader> resourceReader);
   
     std::vector<float> GetPositionBuffer(U32 meshId, U32 meshPrimitive, U32& bufferSize, U32& count) const;
     std::vector<float> GetNormalBuffer(U32 meshId, U32 meshPrimitive, U32& bufferSize, U32& count) const;
     std::vector<float> GetUVBuffer(U32 meshId, U32 meshPrimitive, U32& bufferSize, U32& count) const;
     std::vector<U32> GetIndexBuffer(U32 meshId, U32 meshPrimitive, U32& bufferSize, U32& count) const;
   
   private:
     const Microsoft::glTF::Mesh& GetMesh(U32 idx) const;
     static const Microsoft::glTF::MeshPrimitive& GetMeshPrimitive(const Microsoft::glTF::Mesh& mesh, U32 idx);
   
     std::unique_ptr<const Microsoft::glTF::GLTFResourceReader> m_resourceReader;
     Microsoft::glTF::Document m_document;
   };
   
   namespace GLTFLoader
   {
     GLTFMeshInterface LoadFromJSON(const String& assetName, AssetLocation location, const Log& log);
   
   } // namespace GLTFLoader
   
   } // namespace Azura
