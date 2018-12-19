
.. _program_listing_file_Source_Math_Src_TransformComponent.cpp:

Program Listing for File TransformComponent.cpp
===============================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Math_Src_TransformComponent.cpp>` (``Source\Math\Src\TransformComponent.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Math/TransformComponent.h"
   
   
   namespace Azura {
   namespace Math {
   
   TransformComponent::TransformComponent() : m_transform(Matrix4f::Identity()) {
   }
   
   TransformComponent::TransformComponent(const Matrix4f& transform) : m_transform(transform) {
   }
   
   TransformComponent TransformComponent::FromTranslationVector(const Vector3f& translation) {
   
     return TransformComponent(Matrix4f::FromTranslationVector(translation));
   }
   
   void TransformComponent::SetStepSize(float value) {
     m_stepSize = value;
   }
   
   void TransformComponent::OnKeyEvent(KeyEvent keyEvent) {
     if (keyEvent.m_internalType == KeyEventType::KeyPress) {
       if (m_forwardKey == keyEvent.m_key)
       {
         m_moveForwardFactor = 1;
       }
       
       if (m_backwardKey == keyEvent.m_key)
       {
         m_moveForwardFactor = -1;
       }
   
       if (m_leftKey == keyEvent.m_key)
       {
         m_moveRightFactor = 1;
       }
       
       if (m_rightKey == keyEvent.m_key)
       {
         m_moveRightFactor = -1;
       }
   
       if (m_upKey == keyEvent.m_key)
       {
         m_moveUpFactor = 1;
       }
   
       if (m_downKey == keyEvent.m_key)
       {
         m_moveUpFactor = -1;
       }
   
     } else if (keyEvent.m_internalType == KeyEventType::KeyRelease) {
       if (m_forwardKey == keyEvent.m_key)
       {
         m_moveForwardFactor = 0;
       }
       
       if (m_backwardKey == keyEvent.m_key)
       {
         m_moveForwardFactor = 0;
       }
   
       if (m_leftKey == keyEvent.m_key)
       {
         m_moveRightFactor = 0;
       }
       
       if (m_rightKey == keyEvent.m_key)
       {
         m_moveRightFactor = 0;
       }
   
       if (m_upKey == keyEvent.m_key)
       {
         m_moveUpFactor = 0;
       }
   
       if (m_downKey == keyEvent.m_key)
       {
         m_moveUpFactor = 0;
       }
     }
   }
   
   void TransformComponent::Update(float timeDelta) {
     const float distance = m_stepSize * m_sensitivity * timeDelta;
   
     Vector4f translation = Vector4f(0, 0, 0, 1);
   
     if (m_moveForwardFactor != 0) {
       translation += Vector4f(0, 0, m_moveForwardFactor * distance, 0);
     }
   
     if (m_moveRightFactor != 0) {
       translation += Vector4f(m_moveRightFactor * distance, 0, 0, 0);
     }
   
     if (m_moveUpFactor != 0) {
       translation += Vector4f(0, m_moveUpFactor * distance, 0, 0);
     }
   
     m_transform = Matrix4f::FromTranslationVector(translation.xyz());
   }
   
   void TransformComponent::SetUpKey(KeyboardKey key) {
     m_upKey = key;
   }
   
   void TransformComponent::SetDownKey(KeyboardKey key) {
     m_downKey = key;
   }
   
   void TransformComponent::SetLeftKey(KeyboardKey key) {
     m_leftKey = key;
   }
   
   void TransformComponent::SetRightKey(KeyboardKey key) {
     m_rightKey = key;
   }
   
   void TransformComponent::SetForwardKey(KeyboardKey key) {
     m_forwardKey = key;
   }
   
   void TransformComponent::SetBackwardKey(KeyboardKey key) {
     m_backwardKey = key;
   }
   
   const Matrix4f& TransformComponent::GetTransform() const {
     return m_transform;
   }
   } // namespace Math
   } // namespace Azura
