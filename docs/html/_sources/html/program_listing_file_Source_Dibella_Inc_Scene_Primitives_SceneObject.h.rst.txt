
.. _program_listing_file_Source_Dibella_Inc_Scene_Primitives_SceneObject.h:

Program Listing for File SceneObject.h
======================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Dibella_Inc_Scene_Primitives_SceneObject.h>` (``Source\Dibella\Inc\Scene\Primitives\SceneObject.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "../../Core/Types.h"
   
   namespace Dibella {
   class Allocator;
   class Ray;
   class Intersection;
   class Shape;
   class Material;
   class Light;
   
   class SceneObject {
    public:
     SceneObject();
     SceneObject(Shape* shape, Material* material = nullptr, Light* light = nullptr);
   
     // Returns whether or not the given Ray intersects this SceneObject.
     // Passes additional intersection data through the Intersection pointer
     bool Intersect(const Ray& r, Intersection* isect) const;
   
     const Light* GetLight() const;
     const Material* GetMaterial() const;
   
     // Ask our _material_ to generate a BSDF containing
     // BxDFs and store it in isect.
     bool GenerateBSDF(Intersection* isect, Allocator* allocator) const;
   
    protected:
     // Debug Name
     DString mName;
   
     // Pointer to Shape of SceneObject
     Shape* mShape;
   
     // Pointer to Material of SceneObject
     Material* mMaterial;
   
     // Pointer to Light of SceneObject
     Light* mLight;
   };
   }  // namespace Dibella
