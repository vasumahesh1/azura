
.. _program_listing_file_Source_Physics_Inc_Physics_Geometry_ClothPlane.h:

Program Listing for File ClothPlane.h
=====================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Physics_Inc_Physics_Geometry_ClothPlane.h>` (``Source\Physics\Inc\Physics\Geometry\ClothPlane.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include <Math/Core.h>
   #include <Core/RawStorageFormat.h>
   #include <Physics/Geometry/Edge.h>
   #include <Physics/PBD/Cloth/ClothSolvingView.h>
   #include <Physics/Geometry/GenericTypes.h>
   #include <Physics/Geometry/IClothPhysicsGeometry.h>
   
   #include <vector>
   #include <unordered_map>
   
   namespace Azura {
   namespace Physics {
   struct Edge;
   
   class ClothPlane final : public IClothPhysicsGeometry {
   public:
     ClothPlane(const Vector2f& boundMin , const Vector2f& boundMax, Memory::Allocator& allocator);
     ClothPlane(ClothTriangulation triangulation, const Vector2f& boundMin , const Vector2f& boundMax, float worldHeight, const Vector2u& subDivisions, const Vector2u& uvScale, Memory::Allocator& allocator);
   
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
     PBD::ClothSolvingView GetPBDSolvingView(Memory::Allocator& allocator) override;
   
   private:
     void AddEdgeTriangleNeighbor(const Edge& edge, const U32 triangleIdx);
   
     std::vector<U32> m_anchorIdx;
   
     std::unordered_map<Edge, std::vector<U32>, EdgeMapHash> m_edgeTriangleMap;
   
     Containers::Vector<Vector3f> m_vertices;
     Containers::Vector<float> m_vertexInvMass;
     Containers::Vector<Vector3f> m_normals;
     Containers::Vector<Vector3u> m_triangles;
     Containers::Vector<Vector2f> m_uv;
   };
   
   } // namespace Physics
   } // namespace Azura
