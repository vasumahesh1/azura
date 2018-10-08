#pragma once

namespace Azura {

enum class EventType {
  MouseEvent
};

enum class MouseEventType {
  LeftClick,
  RightClick,
  Drag,
  ScrollUp,
  ScrollDown
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

} // namespace Azura
