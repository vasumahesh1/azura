#pragma once

#include <Physics/PBD/Common/SolvingView.h>
#include <Physics/PBD/Cloth/Constraints.h>

namespace Azura {
namespace Physics {
namespace PBD {

class ClothSolvingView final : public SolvingView {

public:
  explicit ClothSolvingView(Containers::Vector<Vector3f>& dataSet,
                            Containers::Vector<float>& invMassData,
                            U32 numDistanceConstraints,
                            U32 numLongRangeConstraints,
                            U32 numBendingConstraints,
                            Memory::Allocator& allocator);

  void AddConstraint(const DistanceConstraint& constraint);
  void AddConstraint(DistanceConstraint&& constraint);
  
  void AddConstraint(const LongRangeConstraint& constraint);
  void AddConstraint(LongRangeConstraint&& constraint);

  void AddConstraint(const BendingConstraint& constraint);
  void AddConstraint(BendingConstraint&& constraint);

  const Containers::Vector<DistanceConstraint>& GetDistanceConstraints() const;
  const Containers::Vector<LongRangeConstraint>& GetLongRangeConstraints() const;
  const Containers::Vector<BendingConstraint>& GetBendingConstraints() const;

  void Solve(Containers::Vector<Vector3f>& projectionPoints) override;

private:
  Containers::Vector<DistanceConstraint> m_distanceConstraints;
  Containers::Vector<LongRangeConstraint> m_longRangeConstraints;
  Containers::Vector<BendingConstraint> m_bendingConstraints;
};

} // namespace PBD
} // namespace Physics
} // namespace Azura
