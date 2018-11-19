#include "TestZone/AppRenderer.h"
#include "Core/RawStorageFormat.h"
#include "Generic/RenderSystem.h"
#include "Generic/Shader.h"
#include "Memory/MemoryFactory.h"
#include "Memory/MonotonicAllocator.h"
#include "Math/Transform.h"
#include "Math/Icosphere.h"
#include "Math/Plane.h"

#include <algorithm>
#include <random>

namespace Azura {
using namespace Containers; // NOLINT
using namespace Math;       // NOLINT
namespace {
  struct Vertex {
    float m_pos[4];
    float m_col[4];
  };

  struct VertexWithUV {
    float m_pos[4];
    float m_uv[2];
  };

  struct LightData {
    Vector4f m_lightPos;
  };

  constexpr U32 CLOTH_DIV_X = 10;
  constexpr U32 CLOTH_DIV_Y = 10;
} // namespace

AppRenderer::AppRenderer()
  : m_mainBuffer(0x640'0000),
    m_mainAllocator(m_mainBuffer, 0x320'0000),
    m_drawableAllocator(m_mainBuffer, 0x320'0000),
    m_camera(1280, 720),
    m_clothPlane(Vector2f(-3.0f, -3.0f), Vector2f(3.0f, 3.0f), Vector2u(CLOTH_DIV_X, CLOTH_DIV_Y)),
    log_AppRenderer(Log("AppRenderer")) {
}

void AppRenderer::Initialize() {
  LOG_INF(log_AppRenderer, LOG_LEVEL, "Starting Init of AppRenderer");

  HEAP_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 16384);
  m_window = RenderSystem::CreateApplicationWindow("TestZone", 1280, 720);

  m_window->SetUpdateCallback([this](float deltaTime)
  {
    WindowUpdate(deltaTime);
  });

  m_window->SetKeyEventCallback([this](KeyEvent evt)
  {
    m_camera.OnKeyEvent(evt);
  });

  m_window->SetMouseEventCallback([this](MouseEvent evt)
  {
    UNUSED(evt);
    // Disable as it has bugs
    // m_camera.OnMouseEvent(e);
  });

  m_camera.SetZoom(10);
  m_camera.RotateAboutRight(-30);

  VERIFY_TRUE(log_AppRenderer, m_window->Initialize(), "Cannot Initialize Window");

  ApplicationInfo appInfo;
  appInfo.m_name    = "TestZone";
  appInfo.m_version = Version(1, 0, 0);

  DeviceRequirements requirements;
  requirements.m_discreteGPU   = true;
  requirements.m_float64       = false;
  requirements.m_int64         = false;
  requirements.m_transferQueue = false;

  m_camera.Recompute();

  m_clothUBO                     = {};
  m_clothUBO.m_model             = Matrix4f::FromTranslationVector(Vector3f(0, 3, 0));
  m_clothUBO.m_view              = m_camera.GetViewMatrix();
  m_clothUBO.m_viewProj          = m_camera.GetViewProjMatrix();
  m_clothUBO.m_invViewProj       = m_camera.GetInvViewProjMatrix();
  m_clothUBO.m_invProj           = m_camera.GetProjMatrix().Inverse();
  m_clothUBO.m_modelInvTranspose = m_clothUBO.m_model.Inverse().Transpose();

  m_sphereUBO                     = {};
  m_sphereUBO.m_model             = Matrix4f::Identity();
  m_sphereUBO.m_viewProj          = m_camera.GetViewProjMatrix();
  m_sphereUBO.m_view              = m_camera.GetViewMatrix();
  m_sphereUBO.m_invViewProj       = m_camera.GetInvViewProjMatrix();
  m_sphereUBO.m_invProj           = m_camera.GetProjMatrix().Inverse();
  m_sphereUBO.m_modelInvTranspose = m_sphereUBO.m_model.Inverse().Transpose();

  LightData lightData  = {};
  lightData.m_lightPos = Vector4f(0.0f, 15.0f, 0.0f, 1.0f);

  // TODO(vasumahesh1):[Q]:Allocator?
  const ApplicationRequirements applicationRequirements = {};

  TextureRequirements textureRequirements = {};
  textureRequirements.m_maxCount          = 1;
  textureRequirements.m_poolSize          = 0x400000; // 4MB

  DescriptorRequirements descriptorRequirements = DescriptorRequirements(3, 3, allocatorTemporary);
  // SET 0
  const U32 UBO_SLOT                            = descriptorRequirements.AddDescriptor({
    DescriptorType::UniformBuffer, ShaderStage::Vertex
  });
  const U32 LIGHT_SLOT = descriptorRequirements.AddDescriptor({
    DescriptorType::UniformBuffer, ShaderStage::Vertex
  });

  // SET 0 Compute
  m_computePass.m_computeUBOSlot = descriptorRequirements.AddDescriptor({
    DescriptorType::UniformBuffer, ShaderStage::Compute
  });

  const U32 UBO_SET   = descriptorRequirements.AddSet({UBO_SLOT});
  const U32 LIGHT_SET = descriptorRequirements.AddSet({LIGHT_SLOT});
  
  const U32 COMPUTE_UBO_SET = descriptorRequirements.AddSet({m_computePass.m_computeUBOSlot});

  ShaderRequirements shaderRequirements = ShaderRequirements(3, allocatorTemporary);
  const U32 COMPUTE_SHADER_ID            = shaderRequirements.AddShader({
    ShaderStage::Compute, "Cloth.cs", AssetLocation::Shaders
    });
  
  const U32 VERTEX_SHADER_ID            = shaderRequirements.AddShader({
    ShaderStage::Vertex, "Cloth.vs", AssetLocation::Shaders
  });
  const U32 PIXEL_SHADER_ID = shaderRequirements.AddShader({
    ShaderStage::Pixel, "Cloth.ps", AssetLocation::Shaders
  });

  RenderPassRequirements renderPassRequirements = RenderPassRequirements(1, 2, 7, allocatorTemporary);
  renderPassRequirements.m_maxPools             = 2;

  const U32 DISTANCE_CONSTRAINTS_BUFFER = renderPassRequirements.AddBuffer({
    U32(sizeof(DistanceConstraint)) * U32(m_clothPlane.GetEdgeConstraints().size()), U32(sizeof(DistanceConstraint))
    });

  const U32 COMPUTE_VERTEX_BUFFER = renderPassRequirements.AddBuffer({
    U32(sizeof(Vector4f)) * U32(m_clothPlane.GetVertices().size()), U32(sizeof(Vector4f))
    });

  const U32 COMPUTE_PROJECTION_BUFFER = renderPassRequirements.AddBuffer({
    U32(sizeof(Vector4f)) * U32(m_clothPlane.GetVertices().size()), U32(sizeof(Vector4f))
    });

  const U32 COMPUTE_VERTEX_VELOCITY = renderPassRequirements.AddBuffer({
    U32(sizeof(Vector4f)) * U32(m_clothPlane.GetVertices().size()), U32(sizeof(Vector4f))
    });

  // const U32 COMPUTE_VERTEX_LOCK = renderPassRequirements.AddBuffer({
  //   U32(sizeof(U32)) * U32(m_clothPlane.GetVertices().size()), U32(sizeof(U32))
  //   });

  const U32 COMPUTE_VERTEX_CONSTRAINT_COUNT = renderPassRequirements.AddBuffer({
    U32(sizeof(U32)) * U32(m_clothPlane.GetVertices().size()), U32(sizeof(U32))
    });

  const U32 COMPUTE_VERTEX_DELTAX = renderPassRequirements.AddBuffer({
    U32(sizeof(U32)) * U32(m_clothPlane.GetVertices().size()), U32(sizeof(U32))
    });

  const U32 COMPUTE_VERTEX_DELTAY = renderPassRequirements.AddBuffer({
    U32(sizeof(U32)) * U32(m_clothPlane.GetVertices().size()), U32(sizeof(U32))
    });

  const U32 COMPUTE_VERTEX_DELTAZ = renderPassRequirements.AddBuffer({
    U32(sizeof(U32)) * U32(m_clothPlane.GetVertices().size()), U32(sizeof(U32))
    });

  // const U32 COMPUTE_VERTEX_DELTA2 = renderPassRequirements.AddBuffer({
  //   U32(sizeof(Vector4f)) * U32(m_clothPlane.GetVertices().size()), U32(sizeof(Vector4f))
  //   });

  m_computePass.m_passId = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{COMPUTE_SHADER_ID},
    PipelinePassCreateInfo::InputTargets{},
    PipelinePassCreateInfo::InputBuffers{{DISTANCE_CONSTRAINTS_BUFFER, ShaderStage::Compute}},
    PipelinePassCreateInfo::OutputTargets{},
    PipelinePassCreateInfo::OutputBuffers{COMPUTE_VERTEX_BUFFER, COMPUTE_PROJECTION_BUFFER, COMPUTE_VERTEX_VELOCITY, COMPUTE_VERTEX_CONSTRAINT_COUNT, COMPUTE_VERTEX_DELTAX, COMPUTE_VERTEX_DELTAY, COMPUTE_VERTEX_DELTAZ},
    PipelinePassCreateInfo::DescriptorSets{COMPUTE_UBO_SET},
    ClearData{{0.0f, 0.0f, 0.0f, 1.0f}, 1.0f, 0},
    BlendState{},
    RenderPassType::Compute
    });

  const U32 RENDER_PASS = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{VERTEX_SHADER_ID, PIXEL_SHADER_ID}, // SHADERS
    PipelinePassCreateInfo::InputTargets{},                             // INPUT TARGETS
    PipelinePassCreateInfo::InputBuffers{},                             // INPUT TARGETS
    PipelinePassCreateInfo::OutputTargets{},                                  // OUTPUT TARGETS
    PipelinePassCreateInfo::OutputBuffers{},                                  // OUTPUT TARGETS
    PipelinePassCreateInfo::DescriptorSets{UBO_SET, LIGHT_SET},         // DESCRIPTORS
    ClearData{{0.2f, 0.2f, 0.2f, 1.0f}, 1.0f, 0}
  });

  m_clothVertexVel.resize(m_clothPlane.GetVertices().size());
  m_clothProjectedPos.resize(m_clothPlane.GetVertices().size());

  std::vector<Vector4f> zeroBufferData = std::vector<Vector4f>(m_clothPlane.GetVertexCount(), Vector4f(0.0f));
  std::vector<Vector4i> zeroIntVecData = std::vector<Vector4i>(m_clothPlane.GetVertexCount(), Vector4i(0));
  std::vector<U32> zeroIntBuffer = std::vector<U32>(m_clothPlane.GetVertexCount(), 0);

  const SizeType totalConstraints = m_clothPlane.GetEdgeConstraints().size() + m_clothPlane.GetBendingConstraints().size();
  for(SizeType i = 0; i < totalConstraints; ++i)
  {
    m_clothConstraintsIdx.push_back(i);
  }

  for (auto& velocity : m_clothVertexVel) {
    velocity = Vector4f(0, 0, 0, 0);
  }

  m_renderer = RenderSystem::CreateRenderer(appInfo, requirements, applicationRequirements,
                                            m_window->GetSwapChainRequirements(), renderPassRequirements,
                                            descriptorRequirements, shaderRequirements, m_mainAllocator,
                                            m_drawableAllocator,
                                            *m_window);

  m_textureManager = RenderSystem::CreateTextureManager(textureRequirements);

  const auto clothDataStart = reinterpret_cast<const U8*>(m_clothPlane.GetVertices().data()); // NOLINT
  m_renderer->BindBufferTarget(COMPUTE_VERTEX_BUFFER, clothDataStart);
  
  const auto projectionStart = reinterpret_cast<const U8*>(zeroBufferData.data()); // NOLINT
  m_renderer->BindBufferTarget(COMPUTE_PROJECTION_BUFFER, projectionStart);
  
  const auto distanceConstraintStart = reinterpret_cast<const U8*>(m_clothPlane.GetEdgeConstraints().data()); // NOLINT
  m_renderer->BindBufferTarget(DISTANCE_CONSTRAINTS_BUFFER, distanceConstraintStart);

  const auto velocityStart = reinterpret_cast<const U8*>(m_clothVertexVel.data()); // NOLINT
  m_renderer->BindBufferTarget(COMPUTE_VERTEX_VELOCITY, velocityStart);
  
  const auto bufferLock = reinterpret_cast<const U8*>(zeroIntBuffer.data()); // NOLINT
  // m_renderer->BindBufferTarget(COMPUTE_VERTEX_LOCK, bufferLock);
  m_renderer->BindBufferTarget(COMPUTE_VERTEX_CONSTRAINT_COUNT, bufferLock);
  
  const auto deltaStart = reinterpret_cast<const U8*>(zeroIntBuffer.data()); // NOLINT
  m_renderer->BindBufferTarget(COMPUTE_VERTEX_DELTAX, deltaStart);
  m_renderer->BindBufferTarget(COMPUTE_VERTEX_DELTAY, deltaStart);
  m_renderer->BindBufferTarget(COMPUTE_VERTEX_DELTAZ, deltaStart);
  // m_renderer->BindBufferTarget(COMPUTE_VERTEX_DELTA2, deltaStart);

  const U32 numBlocks = (U32(m_clothPlane.GetEdgeConstraints().size()) + BLOCK_SIZE_X - 1) / BLOCK_SIZE_X;

  ComputePoolCreateInfo computePoolInfo = {allocatorTemporary};
  computePoolInfo.m_byteSize            = 0xF00000;
  computePoolInfo.m_computePasses       = {{m_computePass.m_passId}, allocatorTemporary};
  computePoolInfo.m_launchDims          = ThreadGroupDimensions{numBlocks, 1, 1};

  ComputePool& computePool = m_renderer->CreateComputePool(computePoolInfo);
  m_computePool            = &computePool;

  const float distanceStiffness = 0.5f;
  const float distanceStiffnessPrime = 1.0f - std::pow(1.0f - distanceStiffness, 1.0f / SOLVER_ITERATIONS);

  const float bendingStiffness = 0.3f;
  const float bendingStiffnessPrime = 1.0f - std::pow(1.0f - bendingStiffness, 1.0f / SOLVER_ITERATIONS);

  m_computeUBO = {};
  m_computeUBO.m_numVertices = m_clothPlane.GetVertexCount();
  m_computeUBO.m_stretchStiffness = distanceStiffnessPrime;
  m_computeUBO.m_bendStiffness = bendingStiffnessPrime;
  m_computeUBO.m_timeDelta = 0.016667f;
  m_computeUBO.m_numBlocks = numBlocks;
  m_computeUBO.m_numStretchConstraints = U32(m_clothPlane.GetEdgeConstraints().size());
  m_computeUBO.m_numBendConstraints = U32(m_clothPlane.GetBendingConstraints().size());

  const auto computeUBOStart = reinterpret_cast<const U8*>(&m_computeUBO); // NOLINT
  computePool.BindUniformData(m_computePass.m_computeUBOSlot, computeUBOStart, sizeof(ComputeUBO));

  IcoSphere sphere(4);

  DrawablePoolCreateInfo poolInfo = {allocatorTemporary};
  poolInfo.m_byteSize             = sphere.TotalDataSize() + m_clothPlane.TotalDataSize() + 0x400000;
  poolInfo.m_numDrawables         = 2;
  poolInfo.m_renderPasses         = {{RENDER_PASS}, allocatorTemporary};
  poolInfo.m_drawType             = DrawType::InstancedIndexed;

  const auto VERTEX_SLOT = poolInfo.AddInputSlot({
    BufferUsageRate::PerVertex, {{"POSITION", RawStorageFormat::R32G32B32A32_FLOAT}}, 0, BufferSource::StructuredBuffer
  });

  const auto NORMAL_SLOT = poolInfo.AddInputSlot({
    BufferUsageRate::PerVertex, {{"NORMAL", RawStorageFormat::R32G32B32_FLOAT}}
  });

  DrawablePool& pool = m_renderer->CreateDrawablePool(poolInfo);

  const auto uboDataBuffer   = reinterpret_cast<U8*>(&m_clothUBO);  // NOLINT
  const auto sphereUBO       = reinterpret_cast<U8*>(&m_sphereUBO); // NOLINT
  const auto lightDataBuffer = reinterpret_cast<U8*>(&lightData);   // NOLINT
  // Create Drawable from Pool
  DrawableCreateInfo createInfo = {};
  createInfo.m_vertexCount      = m_clothPlane.GetVertexCount();
  createInfo.m_indexCount       = m_clothPlane.GetIndexCount();
  createInfo.m_instanceCount    = 1;
  createInfo.m_indexType        = RawStorageFormat::R32_UINT;

  const auto clothId = pool.CreateDrawable(createInfo);
  pool.BindVertexData(clothId, VERTEX_SLOT, COMPUTE_VERTEX_BUFFER, 0, m_clothPlane.VertexDataSize());
  pool.BindVertexData(clothId, NORMAL_SLOT, m_clothPlane.NormalData(), m_clothPlane.NormalDataSize());
  pool.SetIndexData(clothId, m_clothPlane.IndexData(), m_clothPlane.IndexDataSize());
  pool.BindUniformData(clothId, UBO_SLOT, uboDataBuffer, sizeof(SceneUBO));
  pool.BindUniformData(clothId, LIGHT_SLOT, lightDataBuffer, sizeof(LightData));

  DrawableCreateInfo sphereDrawableInfo = {};
  sphereDrawableInfo.m_vertexCount      = sphere.GetVertexCount();
  sphereDrawableInfo.m_indexCount       = sphere.GetIndexCount();
  sphereDrawableInfo.m_instanceCount    = 1;
  sphereDrawableInfo.m_indexType        = sphere.GetIndexFormat();

  DrawablePoolCreateInfo spherePoolInfo = {allocatorTemporary};
  spherePoolInfo.m_byteSize             = sphere.TotalDataSize() + 0x400000;
  spherePoolInfo.m_numDrawables         = 1;
  spherePoolInfo.m_renderPasses         = {{RENDER_PASS}, allocatorTemporary};
  spherePoolInfo.m_drawType             = DrawType::InstancedIndexed;

  const auto SPHERE_VERTEX_SLOT = spherePoolInfo.AddInputSlot({
    BufferUsageRate::PerVertex, {{"POSITION", RawStorageFormat::R32G32B32A32_FLOAT}}
    });

  const auto SPHERE_NORMAL_SLOT = spherePoolInfo.AddInputSlot({
    BufferUsageRate::PerVertex, {{"NORMAL", RawStorageFormat::R32G32B32_FLOAT}}
    });

  DrawablePool& spherePool = m_renderer->CreateDrawablePool(spherePoolInfo);

  const auto sphereId = spherePool.CreateDrawable(sphereDrawableInfo);
  spherePool.BindVertexData(sphereId, SPHERE_VERTEX_SLOT, sphere.VertexData(), sphere.VertexDataSize());
  spherePool.BindVertexData(sphereId, SPHERE_NORMAL_SLOT, sphere.NormalData(), sphere.NormalDataSize());
  spherePool.SetIndexData(sphereId, sphere.IndexData(), sphere.IndexDataSize());
  spherePool.BindUniformData(sphereId, UBO_SLOT, sphereUBO, sizeof(SceneUBO));
  spherePool.BindUniformData(sphereId, LIGHT_SLOT, lightDataBuffer, sizeof(LightData));

  m_renderPass.m_vertexSlot = VERTEX_SLOT;
  m_renderPass.m_normalSlot = NORMAL_SLOT;
  m_renderPass.m_sceneUBOSlot = UBO_SLOT;
  m_renderPass.m_clothId      = clothId;
  m_renderPass.m_sphereId     = sphereId;

  m_mainPool = &pool;
  m_spherePool = &spherePool;

  // All Drawables Done
  m_renderer->Submit();

  LOG_INF(log_AppRenderer, LOG_LEVEL, "Initialized AppRenderer");
}

void AppRenderer::WindowUpdate(float timeDelta) {
  m_camera.Update(timeDelta);
  
  timeDelta = 0.0166667f;

  m_clothUBO.m_view              = m_camera.GetViewMatrix();
  m_clothUBO.m_viewProj          = m_camera.GetViewProjMatrix();
  m_clothUBO.m_invViewProj       = m_camera.GetInvViewProjMatrix();
  m_clothUBO.m_invProj           = m_camera.GetProjMatrix().Inverse();
  m_clothUBO.m_modelInvTranspose = m_clothUBO.m_model.Inverse().Transpose();
  
  m_sphereUBO.m_view              = m_camera.GetViewMatrix();
  m_sphereUBO.m_viewProj          = m_camera.GetViewProjMatrix();
  m_sphereUBO.m_invViewProj       = m_camera.GetInvViewProjMatrix();
  m_sphereUBO.m_invProj           = m_camera.GetProjMatrix().Inverse();
  m_sphereUBO.m_modelInvTranspose = m_sphereUBO.m_model.Inverse().Transpose();
  
  m_computeUBO.m_timeDelta = timeDelta;
  
  const auto uboDataBuffer    = reinterpret_cast<U8*>(&m_clothUBO);          // NOLINT
  const auto sphereDataBuffer = reinterpret_cast<U8*>(&m_sphereUBO);         // NOLINT
  const auto computeUBOStart = reinterpret_cast<const U8*>(&m_computeUBO); // NOLINT
  
  m_computePool->BeginUpdates();
  m_computePool->UpdateUniformData(m_computePass.m_computeUBOSlot, computeUBOStart, sizeof(ComputeUBO));
  m_computePool->SubmitUpdates();
  
  m_mainPool->BeginUpdates();
  // Update Cloth
  m_mainPool->UpdateUniformData(m_renderPass.m_clothId, m_renderPass.m_sceneUBOSlot, uboDataBuffer, sizeof(SceneUBO));
  // Update Sphere
  m_mainPool->SubmitUpdates();

  m_spherePool->BeginUpdates();
  m_spherePool->UpdateUniformData(m_renderPass.m_sphereId, m_renderPass.m_sceneUBOSlot, sphereDataBuffer,
    sizeof(SceneUBO));
  m_spherePool->SubmitUpdates();

  m_renderer->RenderFrame();
}

void AppRenderer::Run() const {
  LOG_INF(log_AppRenderer, LOG_LEVEL, "Running AppRenderer");
  m_window->StartListening();
}

void AppRenderer::Destroy() const {
  m_window->Destroy();
}
} // namespace Azura
