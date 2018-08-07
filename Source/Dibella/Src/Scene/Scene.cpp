#include "Scene.h"

namespace Dibella {

  Scene::Scene(Allocator* allocator, SizeType numObjects, SizeType numMaterials,
    SizeType numLights) {}

  void Scene::CreateTestScene() {}
  void Scene::Clear() {}
  bool Scene::Intersect(const Ray& ray, Intersection* isect) const {}
}
