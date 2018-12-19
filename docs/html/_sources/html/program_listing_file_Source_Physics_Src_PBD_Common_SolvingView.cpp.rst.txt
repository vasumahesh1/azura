
.. _program_listing_file_Source_Physics_Src_PBD_Common_SolvingView.cpp:

Program Listing for File SolvingView.cpp
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Physics_Src_PBD_Common_SolvingView.cpp>` (``Source\Physics\Src\PBD\Common\SolvingView.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

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
