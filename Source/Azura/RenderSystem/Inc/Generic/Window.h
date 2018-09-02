#pragma once
#include <functional>
#include "GenericTypes.h"
#include "Types.h"
#include "Renderer.h"

namespace Azura {
class Window {
 public:
  virtual ~Window() = default;
  Window(String title, U32 width, U32 height);
  virtual const void* GetHandle() const;

  Window(const Window& other)     = delete;
  Window(Window&& other) noexcept = delete;
  Window& operator=(const Window& other) = delete;
  Window& operator=(Window&& other) noexcept = delete;

  virtual bool Initialize()     = 0;
  virtual void StartListening() = 0;
  void SetUpdateCallback(std::function<void()> eventUpdate);
  virtual void Destroy() = 0;

  ViewportDimensions GetViewport() const;

  SwapChainRequirement GetSwapChainRequirements() const;

 protected:
  U32 GetWidth() const;
  U32 GetHeight() const;

  void SetHandle(void* window);
  const char* GetTitle() const;

  void CallUpdateFunction() const {
    p_updateFunc();
  }

 private:
  U32 m_width;
  U32 m_height;
  String m_title;
  void* p_windowResource;

  std::function<void()> p_updateFunc;
};
}  // namespace Azura
