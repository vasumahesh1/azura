#include "Renderer_test.h"
#include "Generic/Renderer.h"
#include "Generic/Window.h"
#include "Generic/RenderSystem.h"
#include "RenderTestCases.h"
#include "Generic/Shader.h"
#include "Utils/FileReader.h"
#include "Memory/MemoryFactory.h"

using namespace Azura; // NOLINT

const int UNIFORM_BUFFER_BINDING_ID = 0;
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

const String TEST_SUITE = "RendererTest";

const RawStorageFormat SWAPCHAIN_DEFAULT_FORMAT = RawStorageFormat::B8G8R8A8_UNORM;

const U32 SWAPCHAIN_BYTE_SIZE = WINDOW_WIDTH * WINDOW_HEIGHT * GetFormatSize(SWAPCHAIN_DEFAULT_FORMAT);

std::unique_ptr<Azura::Renderer> CreateDefaultRenderer(Azura::Window& window, Allocator& mainAllocator, Allocator& drawableAllocator)
{
  ApplicationInfo appInfo;
  appInfo.m_name    = TEST_SUITE;
  appInfo.m_version = Version(1, 0, 0);

  DeviceRequirements requirements;
  requirements.m_discreteGPU   = true;
  requirements.m_float64       = false;
  requirements.m_int64         = false;
  requirements.m_transferQueue = false;

  ApplicationRequirements applicationRequirements(mainAllocator);
  applicationRequirements.m_clearColor[0] = 0.2f;
  applicationRequirements.m_clearColor[1] = 0.2f;
  applicationRequirements.m_clearColor[2] = 0.2f;
  applicationRequirements.m_uniformBuffers.Reserve(1);
  applicationRequirements.m_uniformBuffers.PushBack(std::make_pair(ShaderStage::Vertex,
    UniformBufferDesc{sizeof(UniformBufferData), 1, UNIFORM_BUFFER_BINDING_ID}));

  std::unique_ptr<Azura::Renderer> renderer = RenderSystem::CreateRenderer(appInfo, requirements, applicationRequirements,
    window.GetSwapChainRequirements(), mainAllocator, drawableAllocator,
    window);

  return renderer;
}

std::unique_ptr<Azura::Window> CreateDefaultWindow()
{
  std::unique_ptr<Azura::Window> windowPtr = RenderSystem::CreateApplicationWindow(TEST_SUITE, WINDOW_WIDTH, WINDOW_HEIGHT);

  windowPtr->SetUpdateCallback([] ()
  {
  });

  if (!windowPtr->Initialize())
  {
    throw std::runtime_error("Cannot Initialize Window");
  }

  return windowPtr;
}

bool CompareImageBytes(const String& referenceFile, const String& testFile)
{
  HEAP_ALLOCATOR(Temporary, Memory::MonotonicAllocator, SWAPCHAIN_BYTE_SIZE * 4);
  const auto referenceFileContents = FileReader::GetFileContents(referenceFile, allocatorTemporary);
  const auto testFileContents = FileReader::GetFileContents(testFile, allocatorTemporary);

  return std::memcmp(referenceFileContents.Data(), testFileContents.Data(), SWAPCHAIN_BYTE_SIZE) == 0;
}

TEST_F(RendererTest, BasicRenderTest) {
  HeapMemoryBuffer mainBuffer(16384);

  RangeAllocator mainAllocator(mainBuffer, 8192);
  RangeAllocator drawableAllocator(mainBuffer, 8192);

  const auto window = CreateDefaultWindow();
  const auto renderer = CreateDefaultRenderer(*window, mainAllocator, drawableAllocator);

  RenderTestCases::ExecuteBasicRenderTest(*renderer, *window, log_TestRenderer);

  ASSERT_TRUE(CompareImageBytes("./ReferenceImages/BasicRenderTest.data", "./BasicRenderTest.data"));

  window->Destroy();
}

TEST_F(RendererTest, BasicInstancingTest) {
  HeapMemoryBuffer mainBuffer(16384);

  RangeAllocator mainAllocator(mainBuffer, 8192);
  RangeAllocator drawableAllocator(mainBuffer, 8192);

  const auto window = CreateDefaultWindow();
  const auto renderer = CreateDefaultRenderer(*window, mainAllocator, drawableAllocator);

  RenderTestCases::ExecuteBasicInstancingTest(*renderer, *window, log_TestRenderer);

  ASSERT_TRUE(CompareImageBytes("./ReferenceImages/BasicInstancingTest.data", "./BasicInstancingTest.data"));

  window->Destroy();
}
