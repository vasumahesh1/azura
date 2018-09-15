#pragma once
#include "Generic/Renderer.h"
#include <mathfu/matrix.h>

struct UniformBufferData
{
  mathfu::Matrix<float, 4, 4> m_model;
  mathfu::Matrix<float, 4, 4> m_view;
  mathfu::Matrix<float, 4, 4> m_proj;
};

namespace Azura {
class Log;
class Window;
}

namespace RenderTestCases {
  void ExecuteBasicRenderTest(Azura::Renderer& renderer, Azura::Window& window, const Azura::Log& log_TestCase);
} // namespace RenderTestCases