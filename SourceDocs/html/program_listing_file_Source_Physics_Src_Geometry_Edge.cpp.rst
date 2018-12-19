
.. _program_listing_file_Source_Physics_Src_Geometry_Edge.cpp:

Program Listing for File Edge.cpp
=================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Physics_Src_Geometry_Edge.cpp>` (``Source\Physics\Src\Geometry\Edge.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Physics/Geometry/Edge.h"
   
   namespace Azura {
   namespace Physics {
   
   bool Edge::operator==(const Edge& rhs) const {
     return ((m_indexA == rhs.m_indexA) && (m_indexB == rhs.m_indexB)) || (
              (m_indexA == rhs.m_indexB) && (m_indexB == rhs.m_indexA));
   }
   
   } // namespace Physics
   } // namespace Azura
