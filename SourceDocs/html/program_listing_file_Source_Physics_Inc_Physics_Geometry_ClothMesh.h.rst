
.. _program_listing_file_Source_Physics_Inc_Physics_Geometry_ClothMesh.h:

Program Listing for File ClothMesh.h
====================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Physics_Inc_Physics_Geometry_ClothMesh.h>` (``Source\Physics\Inc\Physics\Geometry\ClothMesh.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <Math/Core.h>
   #include <Core/RawStorageFormat.h>
   #include <Physics/Geometry/Edge.h>
   #include <Physics/PBD/Cloth/ClothSolvingView.h>
   #include <Physics/Geometry/IClothPhysicsGeometry.h>
   
   #include <vector>
   #include <unordered_map>
   #include "AssetManager/GLTFLoader.h"
   
   namespace Azura {
   class Log;
   enum class AssetLocation;
   } // namespace Azura
   
   namespace Azura {
   namespace Physics {
   struct Edge;
   
   class ClothMesh final : public IClothPhysicsGeometry {
   public:
     ClothMesh(const String& assetName, AssetLocation location, Memory::Allocator& allocator, const Log& log);
   
     U32 VertexDataSize() const override;
     U32 IndexDataSize() const override;
     const U8* VertexData() const override;
     const U8* IndexData() const override;
     RawStorageFormat GetVertexFormat() const override;
     RawStorageFormat GetIndexFormat() const override;
     U32 GetVertexCount() const override;
     U32 GetIndexCount() const override;
     U32 TotalDataSize() const override;
     U32 NormalDataSize() const override;
     const U8* NormalData() const override;
     RawStorageFormat GetNormalFormat() const override;
   
     U32 UVDataSize() const override;
     const U8* UVData() const override;
     RawStorageFormat GetUVFormat() const override;
   
     // Cloth Physics Overrides
     void SetAnchorOnIndex(U32 idx) override;
     const Containers::Vector<float>& GetVertexInverseMass() const override;
     const Containers::Vector<int>& GetVertexAliases() const;
     PBD::ClothSolvingView GetPBDSolvingView(Memory::Allocator& allocator) override;
   
   private:
     void AddEdgeTriangleNeighbor(const Edge& edge, const U32 triangleIdx);
     bool IsVertexAnchorPoint(SizeType idx) const;
   
     std::vector<U32> m_anchorIdx;
   
     std::unordered_map<Edge, std::vector<U32>, EdgeMapHash> m_edgeTriangleMap;
     std::unordered_map<Vector3f, U32, Vector3fHash> m_vertexAliasMap;
   
     Containers::Vector<Vector3f> m_vertices;
     Containers::Vector<float> m_vertexInvMass;
     Containers::Vector<int> m_vertexAlias;
     Containers::Vector<Vector3f> m_normals;
     Containers::Vector<Vector3u> m_triangles;
     Containers::Vector<Vector2f> m_uv;
   
     std::unique_ptr<GLTFMeshInterface> p_interface{nullptr};
   };
   
   } // namespace Physics
   } // namespace Azura
