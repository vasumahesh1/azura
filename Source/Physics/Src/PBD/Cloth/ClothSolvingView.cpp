#include "Physics/PBD/Cloth/ClothSolvingView.h"

namespace Azura {
namespace Physics {
namespace PBD {

ClothSolvingView::ClothSolvingView(Containers::Vector<Vector3f>& dataSet, Containers::Vector<float>& invMassData ,
                                   U32 numDistanceConstraints,
                                   U32 numBendingConstraints,
                                   Memory::Allocator& allocator)
  : SolvingView(dataSet, invMassData),
    m_distanceConstraints(numDistanceConstraints, allocator),
    m_bendingConstraints(numBendingConstraints, allocator) {
}

void ClothSolvingView::AddConstraint(const DistanceConstraint& constraint) {
  m_distanceConstraints.PushBack(constraint);
}

void ClothSolvingView::AddConstraint(DistanceConstraint&& constraint) {
  m_distanceConstraints.PushBack(std::move(constraint));
}

void ClothSolvingView::AddConstraint(const BendingConstraint& constraint) {
  m_bendingConstraints.PushBack(constraint);
}

void ClothSolvingView::AddConstraint(BendingConstraint&& constraint) {
  m_bendingConstraints.PushBack(std::move(constraint));

}

const Containers::Vector<DistanceConstraint>& ClothSolvingView::GetDistanceConstraints() const {
  return m_distanceConstraints;
}

const Containers::Vector<BendingConstraint>& ClothSolvingView::GetBendingConstraints() const {
  return m_bendingConstraints;
}

void ClothSolvingView::Solve(Containers::Vector<Vector3f>& projectionPoints) {
  UNUSED(projectionPoints);
}
} // namespace PBD
} // namespace Physics
} // namespace Azura
