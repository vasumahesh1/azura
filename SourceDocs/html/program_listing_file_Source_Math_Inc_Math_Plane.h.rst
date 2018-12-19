
.. _program_listing_file_Source_Math_Inc_Math_Plane.h:

Program Listing for File Plane.h
================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Math_Inc_Math_Plane.h>` (``Source\Math\Inc\Math\Plane.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "Math/Core.h"
   #include "Math/Geometry.h"
   
   #include <vector>
   
   namespace Azura {
   namespace Math {
   
   class Plane final : public Geometry {
   public:
     Plane(const Vector2f& boundMin , const Vector2f& boundMax);
     Plane(const Vector2f& boundMin , const Vector2f& boundMax, const Vector2u& subDivisions, const Vector2u& uvScale);
   
     U32 VertexDataSize() const override;
     U32 IndexDataSize() const override;
     U32 UVDataSize() const override;
   
     const U8* VertexData() const override;
     const U8* IndexData() const override;
     const U8* UVData() const override;
     RawStorageFormat GetVertexFormat() const override;
     RawStorageFormat GetIndexFormat() const override;
     U32 GetVertexCount() const override;
     U32 GetIndexCount() const override;
     U32 TotalDataSize() const override;
   
     const std::vector<Vector4f>& GetVertices() const;
     std::vector<Vector4f>& GetVertices();
   
     const std::vector<Vector3f>& GetNormals() const;
     std::vector<Vector3f>& GetNormals();
   
     U32 NormalDataSize() const override;
     const U8* NormalData() const override;
     RawStorageFormat GetNormalFormat() const override;
     RawStorageFormat GetUVFormat() const override;
     
   private:
     std::vector<Vector4f> m_vertices;
     std::vector<Vector2f> m_uv;
     std::vector<Vector3f> m_normals{};
     std::vector<Vector3u> m_triangles;
   };
   
   } // namespace Math
   } // namespace Azura
