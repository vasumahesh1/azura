#pragma once

#include <Math/Core.h>
#include <boost/container_hash/hash.hpp>

namespace Azura {
namespace Physics {
struct Edge {
  U32 m_indexA;
  U32 m_indexB;

  bool operator==(const Edge& rhs) const;
};

struct EdgeMapHash {
  std::size_t operator ()(const Edge& e) const {
    return boost::hash_value(std::make_pair(e.m_indexA, e.m_indexB)) + boost::
           hash_value(std::make_pair(e.m_indexB, e.m_indexA));
  }
};

struct Vector3fHash {
  std::size_t operator ()(const Vector3f& v) const {
    return boost::hash_value(v[0]) + boost::hash_value(v[1]) + boost::hash_value(v[2]);
  }
};

} // namespace Physics
} // namespace Azura
