﻿#include "SceneObject.h"

#include "../../Materials/Primitives/Material.h"
#include "../../RayTracing/Primitives/Intersection.h"
#include "Shape.h"

namespace Dibella {
SceneObject::SceneObject() : mName("Some SceneObject"), mShape(nullptr), mMaterial(nullptr), mLight(nullptr) {}

SceneObject::SceneObject(Shape* shape, Material* material, Light* light)
    : mShape(shape), mMaterial(material), mLight(light) {}

bool SceneObject::Intersect(const Ray& r, Intersection* isect) const {
  if (!mShape->Intersect(r, isect)) {
    return false;
  }

  isect->mTargetHit = this;
  return true;
}

const Light* SceneObject::GetLight() const {
  return mLight;
}

const Material* SceneObject::GetMaterial() const {
  return mMaterial;
}

bool SceneObject::GenerateBSDF(Intersection* isect, Allocator* allocator) const {
  if (mMaterial) {
    mMaterial->GenerateBSDF(isect, allocator);
    return true;
  }

  return false;
}
}  // namespace Dibella
