#pragma once
#include "Generic/Renderer.h"
#include "Math/Core.h"


namespace Azura {
class Log;
class Window;

struct UniformBufferData {
  Matrix4f m_model;
  Matrix4f m_modelInvTranspose;
  Matrix4f m_viewProj;
};
}

namespace RenderTestCases {
  void ExecuteBasicRenderTest(Azura::Renderer& renderer, Azura::Window& window, const Azura::Log& log_TestCase);
  void ExecuteBasicInstancingTest(Azura::Renderer& renderer, Azura::Window& window, const Azura::Log& log_TestCase);
  void ExecuteBasicTextureTest(Azura::Renderer& renderer, Azura::Window& window, const Azura::Log& log_TestCase);
} // namespace RenderTestCases