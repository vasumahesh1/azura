
.. _program_listing_file_Source_Dibella_Inc_Scene_Scene.h:

Program Listing for File Scene.h
================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Dibella_Inc_Scene_Scene.h>` (``Source\Dibella\Inc\Scene\Scene.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "Containers/Vector.h"
   #include "Primitives/SceneObject.h"
   
   namespace Dibella {
   class Allocator;
   class Ray;
   class Intersection;
   
   class Scene {
     using SceneVector    = EngineTools::Vector<SceneObject>;
     using MaterialVector = EngineTools::Vector<Material>;
     using LightVector    = EngineTools::Vector<Light>;
   
    public:
     Scene(Allocator* allocator, SizeType numObjects, SizeType numMaterials, SizeType numLights);
   
     void SetCamera(const Camera& c);
     bool Intersect(const Ray& ray, Intersection* isect) const;
   
    private:
     SceneVector mObjects;
     MaterialVector mMaterials;
     LightVector mLights;
   
     Camera mCamera;
     Film mFilm;
   
     void addSphere(QString name, Transform transform, shared_ptr<Material> material);
     void addCube(QString name, Transform transform, shared_ptr<Material> material);
     void addSquarePlane(QString name, Transform transform, shared_ptr<Material> material);
   };
   }  // namespace Dibella
