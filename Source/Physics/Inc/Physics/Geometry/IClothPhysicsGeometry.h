#pragma once
#include "Physics/Geometry/IPhysicsGeometry.h"

namespace Azura {
namespace Physics {

class IClothPhysicsGeometry : public IPhysicsGeometry {
public:
  virtual void SetAnchorOnIndex(U32 idx) = 0;

  virtual const Containers::Vector<float>& GetVertexInverseMass() const  = 0;

  virtual PBD::ClothSolvingView GetPBDSolvingView(Memory::Allocator& allocator) = 0;
};

} // namespace Physics
} // namespace Azura