
.. _program_listing_file_Source_Physics_Inc_Physics_PBD_Common_SolvingView.h:

Program Listing for File SolvingView.h
======================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Physics_Inc_Physics_PBD_Common_SolvingView.h>` (``Source\Physics\Inc\Physics\PBD\Common\SolvingView.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include <Math/Core.h>
   #include <Containers/Vector.h>
   #include <type_traits>
   
   namespace Azura {
   namespace Physics {
   namespace PBD {
   
   class SolvingView {
   public:
     SolvingView(Containers::Vector<Vector3f>& dataSet, Containers::Vector<float>& invMassData);
     virtual ~SolvingView() = default;
   
     SolvingView(const SolvingView& other) = default;
     SolvingView(SolvingView&& other) noexcept = default;
     SolvingView& operator=(const SolvingView& other) = default;
     SolvingView& operator=(SolvingView&& other) noexcept = default;
   
     virtual void Solve(Containers::Vector<Vector3f>& projectionPoints) = 0;
     const Containers::Vector<Vector3f>& GetDataSet() const;
     Containers::Vector<Vector3f>& GetDataSet();
   
   private:
     std::reference_wrapper<Containers::Vector<Vector3f>> m_dataSet;
     std::reference_wrapper<Containers::Vector<float>> m_invMassData;
   };
   
   } // namespace PBD
   } // namespace Physics
   } // namespace Azura
