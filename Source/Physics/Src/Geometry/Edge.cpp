#include "Physics/Geometry/Edge.h"

namespace Azura {
namespace Physics {

bool Edge::operator==(const Edge& rhs) const {
  return ((m_indexA == rhs.m_indexA) && (m_indexB == rhs.m_indexB)) || (
           (m_indexA == rhs.m_indexB) && (m_indexB == rhs.m_indexA));
}

} // namespace Physics
} // namespace Azura
