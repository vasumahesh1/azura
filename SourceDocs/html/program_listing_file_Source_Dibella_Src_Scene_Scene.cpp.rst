
.. _program_listing_file_Source_Dibella_Src_Scene_Scene.cpp:

Program Listing for File Scene.cpp
==================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Dibella_Src_Scene_Scene.cpp>` (``Source\Dibella\Src\Scene\Scene.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Scene.h"
   
   namespace Dibella {
   
   Scene::Scene(Allocator* allocator, SizeType numObjects, SizeType numMaterials, SizeType numLights) {}
   
   void Scene::CreateTestScene() {}
   void Scene::Clear() {}
   bool Scene::Intersect(const Ray& ray, Intersection* isect) const {}
   }  // namespace Dibella
