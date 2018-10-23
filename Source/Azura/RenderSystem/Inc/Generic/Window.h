#pragma once
#include <functional>
#include "GenericTypes.h"
#include "Types.h"
#include "Renderer.h"
#include "Core/Events.h"
#include "Log/Log.h"

namespace Azura {
enum class CursorState
{
  Visible,
  Hidden,
  Disabled
};

class Window {
public:
  virtual ~Window() = default;
  Window(String title, U32 width, U32 height);
  virtual const void* GetHandle() const;

  Window(const Window& other) = delete;
  Window(Window&& other) noexcept = delete;
  Window& operator=(const Window& other) = delete;
  Window& operator=(Window&& other) noexcept = delete;

  virtual bool Initialize() = 0;
  virtual void StartListening() = 0;
  void SetUpdateCallback(std::function<void(float)> eventUpdate);
  void SetMouseEventCallback(std::function<void(MouseEvent)> eventFunc);
  void SetKeyEventCallback(std::function<void(KeyEvent)> eventFunc);
  virtual void Destroy() = 0;

  virtual void SetCursorState(CursorState state) = 0;
  virtual void ResetCursor() = 0;

  ViewportDimensions GetViewport() const;

  SwapChainRequirements GetSwapChainRequirements() const;

protected:
  U32 GetWidth() const;
  U32 GetHeight() const;

  void SetHandle(void* window);
  const char* GetTitle() const;

  void CallUpdateFunction(float timeDelta) const;
  void CallMouseEventFunction(MouseEvent e) const;

  void CallKeyEventFunction(KeyEvent e) const;

  Log log_Window;

  float m_midWidth;
  float m_midHeight;

private:
  U32 m_width;
  U32 m_height;

  String m_title;
  void* p_windowResource;

  std::function<void(float)> p_updateFunc;
  std::function<void(MouseEvent)> p_mouseEventFunc{nullptr};
  std::function<void(KeyEvent)> p_keyEventFunc{nullptr};
};
} // namespace Azura
