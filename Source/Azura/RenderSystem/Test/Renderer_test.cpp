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
  HEAP_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 16384);

  HeapMemoryBuffer mainBuffer(16384);

  RangeAllocator mainAllocator(mainBuffer, 8192);
  RangeAllocator drawableAllocator(mainBuffer, 8192);

  const auto window = CreateDefaultWindow();

  ApplicationInfo appInfo;
  appInfo.m_name    = TEST_SUITE;
  appInfo.m_version = Version(1, 0, 0);

  DeviceRequirements requirements;
  requirements.m_discreteGPU   = true;
  requirements.m_float64       = false;
  requirements.m_int64         = false;
  requirements.m_transferQueue = false;

  ApplicationRequirements applicationRequirements = {};
  applicationRequirements.m_clearColor[0] = 0.2f;
  applicationRequirements.m_clearColor[1] = 0.2f;
  applicationRequirements.m_clearColor[2] = 0.2f;

  DescriptorRequirements descriptorRequirements = DescriptorRequirements(3, allocatorTemporary);
  const U32 UBO_SLOT = descriptorRequirements.AddDescriptor({ DescriptorType::UniformBuffer, ShaderStage::Vertex });

  ShaderRequirements shaderRequirements = ShaderRequirements(2, allocatorTemporary);
  const U32 VERTEX_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Vertex, "BasicRenderTest.vs", AssetLocation::Shaders });
  const U32 PIXEL_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Pixel, "BasicRenderTest.ps", AssetLocation::Shaders });

  RenderPassRequirements renderPassRequirements = RenderPassRequirements(0, 1, allocatorTemporary);

  const U32 SINGLE_PASS = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{VERTEX_SHADER_ID, PIXEL_SHADER_ID},  // SHADERS
    PipelinePassCreateInfo::Inputs{},                                    // INPUT TARGETS
    PipelinePassCreateInfo::Outputs{},                     // OUTPUT TARGETS
    PipelinePassCreateInfo::Descriptors{UBO_SLOT}                        // DESCRIPTORS
    });

  std::unique_ptr<Azura::Renderer> renderer = RenderSystem::CreateRenderer(appInfo, requirements, applicationRequirements,
    window->GetSwapChainRequirements(), renderPassRequirements,
    descriptorRequirements, shaderRequirements, mainAllocator, drawableAllocator,
    *window);

  RenderTestCases::ExecuteBasicRenderTest(*renderer, *window, SINGLE_PASS, UBO_SLOT, log_TestRenderer);

  ASSERT_TRUE(CompareImageBytes("./ReferenceImages/BasicRenderTest.data", "./BasicRenderTest.data"));

  window->Destroy();
}

TEST_F(RendererTest, BasicInstancingTest) {
  HEAP_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 16384);

  HeapMemoryBuffer mainBuffer(16384);

  RangeAllocator mainAllocator(mainBuffer, 8192);
  RangeAllocator drawableAllocator(mainBuffer, 8192);

  const auto window = CreateDefaultWindow();

  ApplicationInfo appInfo;
  appInfo.m_name    = TEST_SUITE;
  appInfo.m_version = Version(1, 0, 0);

  DeviceRequirements requirements;
  requirements.m_discreteGPU   = true;
  requirements.m_float64       = false;
  requirements.m_int64         = false;
  requirements.m_transferQueue = false;

  ApplicationRequirements applicationRequirements = {};
  applicationRequirements.m_clearColor[0] = 0.2f;
  applicationRequirements.m_clearColor[1] = 0.2f;
  applicationRequirements.m_clearColor[2] = 0.2f;

  DescriptorRequirements descriptorRequirements = DescriptorRequirements(3, allocatorTemporary);
  const U32 UBO_SLOT = descriptorRequirements.AddDescriptor({ DescriptorType::UniformBuffer, ShaderStage::Vertex });

  ShaderRequirements shaderRequirements = ShaderRequirements(2, allocatorTemporary);
  const U32 VERTEX_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Vertex, "BasicInstancingTest.vs", AssetLocation::Shaders });
  const U32 PIXEL_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Pixel, "BasicInstancingTest.ps", AssetLocation::Shaders });

  RenderPassRequirements renderPassRequirements = RenderPassRequirements(0, 1, allocatorTemporary);

  const U32 SINGLE_PASS = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{VERTEX_SHADER_ID, PIXEL_SHADER_ID},  // SHADERS
    PipelinePassCreateInfo::Inputs{},                                    // INPUT TARGETS
    PipelinePassCreateInfo::Outputs{},                     // OUTPUT TARGETS
    PipelinePassCreateInfo::Descriptors{UBO_SLOT}                        // DESCRIPTORS
    });

  std::unique_ptr<Azura::Renderer> renderer = RenderSystem::CreateRenderer(appInfo, requirements, applicationRequirements,
    window->GetSwapChainRequirements(), renderPassRequirements,
    descriptorRequirements, shaderRequirements, mainAllocator, drawableAllocator,
    *window);

  RenderTestCases::ExecuteBasicInstancingTest(*renderer, *window, SINGLE_PASS, UBO_SLOT, log_TestRenderer);

  ASSERT_TRUE(CompareImageBytes("./ReferenceImages/BasicInstancingTest.data", "./BasicInstancingTest.data"));

  window->Destroy();
}

TEST_F(RendererTest, BasicTextureTest) {
  HEAP_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 16384);

  HeapMemoryBuffer mainBuffer(16384);

  RangeAllocator mainAllocator(mainBuffer, 8192);
  RangeAllocator drawableAllocator(mainBuffer, 8192);

  const auto window = CreateDefaultWindow();

  ApplicationInfo appInfo;
  appInfo.m_name    = TEST_SUITE;
  appInfo.m_version = Version(1, 0, 0);

  DeviceRequirements requirements;
  requirements.m_discreteGPU   = true;
  requirements.m_float64       = false;
  requirements.m_int64         = false;
  requirements.m_transferQueue = false;

  ApplicationRequirements applicationRequirements = {};
  applicationRequirements.m_clearColor[0] = 0.2f;
  applicationRequirements.m_clearColor[1] = 0.2f;
  applicationRequirements.m_clearColor[2] = 0.2f;

  DescriptorRequirements descriptorRequirements = DescriptorRequirements(3, allocatorTemporary);
  const U32 UBO_SLOT = descriptorRequirements.AddDescriptor({ DescriptorType::UniformBuffer, ShaderStage::Vertex });
  const U32 SAMPLER_SLOT = descriptorRequirements.AddDescriptor({DescriptorType::Sampler, ShaderStage::Pixel});
  const U32 BASIC_TEXTURE_SLOT = descriptorRequirements.AddDescriptor({DescriptorType::SampledImage, ShaderStage::Pixel, DescriptorBinding::Same});

  ShaderRequirements shaderRequirements = ShaderRequirements(2, allocatorTemporary);
  const U32 VERTEX_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Vertex, "BasicTextureTest.vs", AssetLocation::Shaders });
  const U32 PIXEL_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Pixel, "BasicTextureTest.ps", AssetLocation::Shaders });

  RenderPassRequirements renderPassRequirements = RenderPassRequirements(0, 1, allocatorTemporary);

  const U32 SINGLE_PASS = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{VERTEX_SHADER_ID, PIXEL_SHADER_ID},  // SHADERS
    PipelinePassCreateInfo::Inputs{},                                    // INPUT TARGETS
    PipelinePassCreateInfo::Outputs{},                     // OUTPUT TARGETS
    PipelinePassCreateInfo::Descriptors{UBO_SLOT, SAMPLER_SLOT, BASIC_TEXTURE_SLOT}                        // DESCRIPTORS
    });

  std::unique_ptr<Azura::Renderer> renderer = RenderSystem::CreateRenderer(appInfo, requirements, applicationRequirements,
    window->GetSwapChainRequirements(), renderPassRequirements,
    descriptorRequirements, shaderRequirements, mainAllocator, drawableAllocator,
    *window);

  RenderTestCases::ExecuteBasicTextureTest(*renderer, *window, SINGLE_PASS, UBO_SLOT, SAMPLER_SLOT, BASIC_TEXTURE_SLOT, log_TestRenderer);

  ASSERT_TRUE(CompareImageBytes("./ReferenceImages/BasicTextureTest.data", "./BasicTextureTest.data"));

  window->Destroy();
}

TEST_F(RendererTest, BasicDeferredTest) {
  HEAP_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 16384);

  HeapMemoryBuffer mainBuffer(16384);

  RangeAllocator mainAllocator(mainBuffer, 8192);
  RangeAllocator drawableAllocator(mainBuffer, 8192);

  const auto window = CreateDefaultWindow();

  ApplicationInfo appInfo;
  appInfo.m_name    = TEST_SUITE;
  appInfo.m_version = Version(1, 0, 0);

  DeviceRequirements requirements;
  requirements.m_discreteGPU   = true;
  requirements.m_float64       = false;
  requirements.m_int64         = false;
  requirements.m_transferQueue = false;

  ApplicationRequirements applicationRequirements = {};
  applicationRequirements.m_clearColor[0] = 0.2f;
  applicationRequirements.m_clearColor[1] = 0.2f;
  applicationRequirements.m_clearColor[2] = 0.2f;

  DescriptorRequirements descriptorRequirements = DescriptorRequirements(3, allocatorTemporary);
  const U32 UBO_SLOT = descriptorRequirements.AddDescriptor({ DescriptorType::UniformBuffer, ShaderStage::Vertex });
  const U32 SAMPLER_SLOT = descriptorRequirements.AddDescriptor({DescriptorType::Sampler, ShaderStage::Pixel});
  const U32 BASIC_TEXTURE_SLOT = descriptorRequirements.AddDescriptor({DescriptorType::SampledImage, ShaderStage::Pixel, DescriptorBinding::Same});

  ShaderRequirements shaderRequirements = ShaderRequirements(2, allocatorTemporary);
  const U32 VERTEX_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Vertex, "BasicDeferredTest.vs", AssetLocation::Shaders });
  const U32 PIXEL_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Pixel, "BasicDeferredTest.ps", AssetLocation::Shaders });

  const U32 DEF_VERTEX_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Vertex, "BasicDeferredTest.Deferred.vs", AssetLocation::Shaders });
  const U32 DEF_PIXEL_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Pixel, "BasicDeferredTest.Deferred.ps", AssetLocation::Shaders });

  RenderPassRequirements renderPassRequirements = RenderPassRequirements(1, 2, allocatorTemporary);
  const U32 COLOR_TARGET_1 = renderPassRequirements.AddTarget({RawStorageFormat::R32G32B32A32_FLOAT});

  const U32 GBUFFER_PASS = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{VERTEX_SHADER_ID, PIXEL_SHADER_ID},  // SHADERS
    PipelinePassCreateInfo::Inputs{},                                    // INPUT TARGETS
    PipelinePassCreateInfo::Outputs{COLOR_TARGET_1},                     // OUTPUT TARGETS
    PipelinePassCreateInfo::Descriptors{UBO_SLOT}                        // DESCRIPTORS
    });

  const U32 SHADING_PASS = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{DEF_VERTEX_SHADER_ID, DEF_PIXEL_SHADER_ID},
    PipelinePassCreateInfo::Inputs{{COLOR_TARGET_1, ShaderStage::Pixel}},
    PipelinePassCreateInfo::Outputs{PRESENT_TARGET}, // END OF RENDERING
    PipelinePassCreateInfo::Descriptors{SAMPLER_SLOT}
    });

  std::unique_ptr<Azura::Renderer> renderer = RenderSystem::CreateRenderer(appInfo, requirements, applicationRequirements,
    window->GetSwapChainRequirements(), renderPassRequirements,
    descriptorRequirements, shaderRequirements, mainAllocator, drawableAllocator,
    *window);

  RenderTestCases::ExecuteBasicDeferredTest(*renderer, *window, GBUFFER_PASS, SHADING_PASS, UBO_SLOT, SAMPLER_SLOT, BASIC_TEXTURE_SLOT, log_TestRenderer);

  ASSERT_TRUE(CompareImageBytes("./ReferenceImages/BasicDeferredTest.data", "./BasicDeferredTest.data"));

  window->Destroy();
}
