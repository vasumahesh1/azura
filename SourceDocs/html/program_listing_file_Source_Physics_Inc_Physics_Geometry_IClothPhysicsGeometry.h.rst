
.. _program_listing_file_Source_Physics_Inc_Physics_Geometry_IClothPhysicsGeometry.h:

Program Listing for File IClothPhysicsGeometry.h
================================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Physics_Inc_Physics_Geometry_IClothPhysicsGeometry.h>` (``Source\Physics\Inc\Physics\Geometry\IClothPhysicsGeometry.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

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
