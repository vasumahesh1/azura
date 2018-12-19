
.. _program_listing_file_Source_Core_Inc_Core_Events.h:

Program Listing for File Events.h
=================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Core_Inc_Core_Events.h>` (``Source\Core\Inc\Core\Events.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   namespace Azura {
   
   enum class EventType {
     MouseEvent,
     KeyEvent
   };
   
   enum class MouseEventType {
     LeftClick,
     RightClick,
     Drag,
     ScrollUp,
     ScrollDown,
     MouseMove,
     MouseUpdate
   };
   
   enum class KeyEventType {
     KeyPress,
     KeyRelease
   };
   
   enum class KeyboardKey {
     Unmapped,
     W,
     A,
     S,
     D,
     Q,
     E,
     R,
     T,
     Y,
     U,
     I,
     Up,
     Down,
     Left,
     Right,
     Esc,
   };
   
   struct Event {
     EventType m_type;
   };
   
   struct MouseEvent final : public Event {
     MouseEventType m_internalType;
     float m_eventX;
     float m_eventY;
   
     MouseEvent()
       : Event({EventType::MouseEvent}),
         m_internalType(MouseEventType::LeftClick),
         m_eventX(0),
         m_eventY(0) {
     }
   };
   
   struct KeyEvent final : public Event {
     int m_keyValue;
     KeyEventType m_internalType;
     KeyboardKey m_key;
   
     KeyEvent(int key, KeyboardKey keyboardKey, KeyEventType eventType)
       : Event({EventType::KeyEvent}),
         m_keyValue(key),
         m_internalType(eventType),
         m_key(keyboardKey) {
   
     }
   };
   
   } // namespace Azura
