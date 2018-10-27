#include "App/App.h"
#include "Generic/RenderSystem.h"
#include <random>

using namespace Azura::Memory; // NOLINT

namespace Azura
{

App::App() :
  log_App(Log("App")),
  m_mainBuffer(0x640'0000),
  m_mainAllocator(m_mainBuffer, 0x320'0000),
  m_drawableAllocator(m_mainBuffer, 0x320'0000),
  m_lights(NUM_LIGHTS, m_mainAllocator),
  m_camera(1280, 720),
  m_forwardScene(m_mainAllocator, m_drawableAllocator),
  m_forwardComputeScene(m_mainAllocator, m_drawableAllocator),
  m_forwardPlusComputeScene(m_mainAllocator, m_drawableAllocator) {
}

void App::Initialize() {

  // Init Sponza Mesh
  m_sponza.p_interface = std::make_unique<GLTFMeshInterface>(GLTFLoader::LoadFromJSON("sponza", AssetLocation::Meshes, log_App));

  m_sponza.m_vertexData = m_sponza.p_interface->GetPositionBuffer(0, 0, m_sponza.m_vertexDataSize, m_sponza.m_numVertices);
  m_sponza.m_normalData = m_sponza.p_interface->GetNormalBuffer(0, 0, m_sponza.m_normalDataSize, m_sponza.m_numNormals);
  m_sponza.m_uvData = m_sponza.p_interface->GetUVBuffer(0, 0, m_sponza.m_uvDataSize, m_sponza.m_numUV);
  m_sponza.m_indexData = m_sponza.p_interface->GetIndexBuffer(0, 0, m_sponza.m_indexDataSize, m_sponza.m_numIndices);

  // Init Light Texture Sampler
  m_lightSamplerDesc.m_filter       = TextureFilter::MinMagMipPoint;
  m_lightSamplerDesc.m_addressModeU = TextureAddressMode::Clamp;
  m_lightSamplerDesc.m_addressModeV = TextureAddressMode::Clamp;
  m_lightSamplerDesc.m_addressModeW = TextureAddressMode::Clamp;

#if defined(FORWARD_SCENE)
  const String appName = "Forward without Compute";
#elif defined(FORWARD_COMPUTE_SCENE)
  const String appName = "Forward with Compute";
#elif defined(FORWARD_PLUS_COMPUTE_SCENE)
  const String appName = "Forward Plus with Compute";
#endif

  // Init Window & Camera
  p_window = RenderSystem::CreateApplicationWindow(appName, 1280, 720);
  VERIFY_TRUE(log_App, p_window->Initialize(), "Cannot Initialize Window");

  p_window->SetCursorState(CursorState::Hidden);

  p_window->SetUpdateCallback([this](float timeDelta)
  {
    Update(timeDelta);
  });

  p_window->SetMouseEventCallback([this](MouseEvent e)
  {
    m_camera.OnMouseEvent(e);
    p_window->ResetCursor();
  });

  p_window->SetKeyEventCallback([this](KeyEvent e)
  {
    m_camera.OnKeyEvent(e);
  });

  m_camera.SetTranslationStepSize(5.0f);
  m_camera.SetSensitivity(0.5f);

  // Trigger a recompute
  m_camera.Recompute();

  // Init UBO Buffer
  m_sceneUBO               = {};
  m_sceneUBO.m_model       = Matrix4f::Identity();
  m_sceneUBO.m_model(1, 3) = -5;

  m_sceneUBO.m_viewProj          = m_camera.GetViewProjMatrix();
  m_sceneUBO.m_modelInvTranspose = m_sceneUBO.m_model.Inverse().Transpose();

  GenerateLights();

  // Init All Scenes here
#if defined(FORWARD_SCENE)
  m_forwardScene.Initialize(*p_window, m_camera, m_sponza, m_sceneUBO, m_lightSamplerDesc, m_lights);
  p_activeScene = &m_forwardScene;
#elif defined(FORWARD_COMPUTE_SCENE)
  m_forwardComputeScene.Initialize(*p_window, m_camera, m_sponza, m_sceneUBO, m_lightSamplerDesc, m_lights);
  p_activeScene = &m_forwardComputeScene;
#elif defined(FORWARD_PLUS_COMPUTE_SCENE)
  m_forwardPlusComputeScene.Initialize(*p_window, m_camera, m_sponza, m_sceneUBO, m_lightSamplerDesc, m_lights);
  p_activeScene = &m_forwardPlusComputeScene;
#endif
}

void App::Update(float timeDelta) {
  for (auto& light : m_lights) {
    light.m_position[1] += (LIGHT_DT * timeDelta);

    // Wrap lights from bottom to top
    light.m_position[1] = std::fmod(light.m_position[1] + LIGHT_MAX[1] - LIGHT_MIN[1], LIGHT_MAX[1] + LIGHT_MIN[1]);
  }

  // Camera Updates
  m_camera.Update(timeDelta);
  m_sceneUBO.m_viewProj = m_camera.GetViewProjMatrix();

  p_activeScene->Update(timeDelta, m_camera, m_sceneUBO, m_lights);
}

void App::Run() const {
  LOG_INF(log_App, LOG_LEVEL, "Running Application!");
  p_window->StartListening();
}

void App::CleanUp() {
  m_forwardScene.CleanUp();
  p_window->Destroy();
}

void App::GenerateLights() {
  std::random_device rd;
  std::mt19937 mt(rd());

  const std::uniform_real_distribution<float> uniformX(LIGHT_MIN[0], LIGHT_MAX[0]);
  const std::uniform_real_distribution<float> uniformY(LIGHT_MIN[1], LIGHT_MAX[1]);
  const std::uniform_real_distribution<float> uniformZ(LIGHT_MIN[2], LIGHT_MAX[2]);

  const std::uniform_real_distribution<float> color(-0.25f, 0.5f);

  for (U32 idx = 0; idx < NUM_LIGHTS; ++idx) {

    PointLight light = {};
    light.m_position = { uniformX(mt), uniformY(mt), uniformZ(mt) };
    light.m_color = { 0.5f, 0.5f, 0.5f };
    light.m_color += Vector3f(color(mt), color(mt), color(mt));
    light.m_radius = LIGHT_RADIUS;

    LOG_DBG(log_App, LOG_LEVEL, "Generating Light:");
    LOG_DBG(log_App, LOG_LEVEL, "\tPos: %f, %f, %f", light.m_position[0], light.m_position[1], light.m_position[2]);
    LOG_DBG(log_App, LOG_LEVEL, "\tCol: %f, %f, %f", light.m_color[0], light.m_color[1], light.m_color[2]);

    m_lights.PushBack(light);
  }
}
} // namespace Azura