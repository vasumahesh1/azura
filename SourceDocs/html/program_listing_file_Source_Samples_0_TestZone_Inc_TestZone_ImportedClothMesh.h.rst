
.. _program_listing_file_Source_Samples_0_TestZone_Inc_TestZone_ImportedClothMesh.h:

Program Listing for File ImportedClothMesh.h
============================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Samples_0_TestZone_Inc_TestZone_ImportedClothMesh.h>` (``Source\Samples\0_TestZone\Inc\TestZone\ImportedClothMesh.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "TestZone/ClothCommon.h"
   
   #include "Math/Core.h"
   #include "Math/Geometry.h"
   
   namespace Azura {
   
   class ImportedClothMesh final : public Math::Geometry {
   public:
     ImportedClothMesh(const Vector2f& boundMin , const Vector2f& boundMax);
     ImportedClothMesh(const Vector2f& boundMin , const Vector2f& boundMax, const Vector2u& subDivisions);
   
     U32 VertexDataSize() const override;
     U32 IndexDataSize() const override;
     const U8* VertexData() const override;
     const U8* IndexData() const override;
     RawStorageFormat GetVertexFormat() const override;
     RawStorageFormat GetIndexFormat() const override;
     U32 GetVertexCount() const override;
     U32 GetIndexCount() const override;
     U32 TotalDataSize() const override;
   
     const std::vector<Vector4f>& GetVertices() const;
     const std::vector<DistanceConstraint>& GetEdgeConstraints() const;
     const std::vector<BendingConstraint>& GetBendingConstraints() const;
     const std::vector<U32>& GetAnchorIds() const;
     std::vector<Vector4f>& GetVertices();
   
     const std::vector<Vector3f>& GetNormals() const;
     std::vector<Vector3f>& GetNormals();
     void AddEdgeConstraint(const DistanceConstraint & e);
   
     U32 NormalDataSize() const override;
     const U8* NormalData() const override;
     RawStorageFormat GetNormalFormat() const override;
   
     U32 UVDataSize() const override;
     const U8* UVData() const override;
     RawStorageFormat GetUVFormat() const override;
   
   private:
     void AddEdgeTriangleNeighbor(const Edge& edge, const U32 triangleIdx);
   
     std::vector<U32> m_anchorIdx;
   
     std::unordered_map<Edge, std::vector<U32>, EdgeMapHash> m_edgeTriangleMap;
   
     std::vector<Vector4f> m_vertices;
     std::vector<Vector3f> m_normals{};
     std::vector<Vector3u> m_triangles;
     std::vector<DistanceConstraint> m_distanceConstraints;
     std::vector<BendingConstraint> m_bendingConstraints;
   };
   
   } // namespace Azura
