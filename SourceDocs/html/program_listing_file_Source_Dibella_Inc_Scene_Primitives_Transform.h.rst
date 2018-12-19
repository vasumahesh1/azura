
.. _program_listing_file_Source_Dibella_Inc_Scene_Primitives_Transform.h:

Program Listing for File Transform.h
====================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Dibella_Inc_Scene_Primitives_Transform.h>` (``Source\Dibella\Inc\Scene\Primitives\Transform.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "../../Core/Types.h"
   
   namespace Dibella {
   
   class Transform {
    public:
     Transform();
     Transform(const Vector3f& t, const Vector3f& r, const Vector3f& s);
   
     const Matrix4f& Get() const;
     Matrix3f GetAsMatrix3() const;
     const Matrix4f& GetInverse() const;
     const Matrix3f& GetInverseTranspose() const;
   
     Vector3f GetPosition() const {
       return mTranslation;
     }
     Vector3f GetRotation() const {
       return mRotation;
     }
     Vector3f GetScale() const {
       return mScale;
     }
   
    private:
     Vector3f mTranslation;
     Vector3f mRotation;
     Vector3f mScale;
   
     Matrix4f mWorldTransform;
     Matrix4f mInverseWorldTransform;
     Matrix3f mInverseTransposeWorldTransform;
   };
   }  // namespace Dibella
