#pragma once
#include "Generic/Renderer.h"
#include "Math/Core.h"


struct UniformBufferData
{
  Azura::Matrix4f m_model;
  Azura::Matrix4f m_view;
  Azura::Matrix4f m_proj;
};

namespace Azura {
class Log;
class Window;
}

namespace RenderTestCases {
  void ExecuteBasicRenderTest(Azura::Renderer& renderer, Azura::Window& window, const Azura::Log& log_TestCase);
} // namespace RenderTestCases