#include "Physics/PBD/Common/SolvingView.h"

namespace Azura {
namespace Physics {
namespace PBD {

SolvingView::SolvingView(Containers::Vector<Vector3f>& dataSet, Containers::Vector<float>& invMassData)
  : m_dataSet(dataSet), m_invMassData(invMassData) {
}

const Containers::Vector<Vector3f>& SolvingView::GetDataSet() const {
  return m_dataSet;
}

Containers::Vector<Vector3f>& SolvingView::GetDataSet() {
  return m_dataSet;
}
} // namespace PBD
} // namespace Physics
} // namespace Azura
