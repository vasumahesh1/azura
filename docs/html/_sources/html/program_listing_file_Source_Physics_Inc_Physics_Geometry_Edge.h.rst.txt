
.. _program_listing_file_Source_Physics_Inc_Physics_Geometry_Edge.h:

Program Listing for File Edge.h
===============================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Physics_Inc_Physics_Geometry_Edge.h>` (``Source\Physics\Inc\Physics\Geometry\Edge.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

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
