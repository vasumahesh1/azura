#include "ClothSim/AppRenderer.h"
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
using namespace Containers;   // NOLINT
using namespace Math;         // NOLINT
using namespace Physics;      // NOLINT
using namespace Physics::PBD; // NOLINT
namespace {
struct LightData {
  Vector4f m_lightPos;
};

constexpr U32 CLOTH_DIV_X = 10;
constexpr U32 CLOTH_DIV_Y = 10;
} // namespace

AppRenderer::AppRenderer()
  : m_mainBuffer(0xC00'0000),
    m_mainAllocator(m_mainBuffer, 0x400'0000),
    m_localAllocator(m_mainBuffer, 0x400'0000),
    m_drawableAllocator(m_mainBuffer, 0x400'0000),
    m_camera(1280, 720),
    m_clothPlane(Physics::ClothTriangulation::Diamond, Vector2f(-3.0f, -3.0f), Vector2f(3.0f, 3.0f),
                 Vector2u(CLOTH_DIV_X, CLOTH_DIV_Y), m_localAllocator),
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
  appInfo.m_name    = "PBD Cloth Simulation";
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

  DescriptorRequirements descriptorRequirements = DescriptorRequirements(4, 4, allocatorTemporary);
  // SET 0
  const U32 UBO_SLOT = descriptorRequirements.AddDescriptor({
    DescriptorType::UniformBuffer, ShaderStage::Vertex
  });
  const U32 LIGHT_SLOT = descriptorRequirements.AddDescriptor({
    DescriptorType::UniformBuffer, ShaderStage::Vertex
  });

  // SET 0 Compute
  m_computePass.m_computeUBOSlot = descriptorRequirements.AddDescriptor({
    DescriptorType::UniformBuffer, ShaderStage::Compute
  });

  // SET 0 Normal Compute
  m_normalsPass.m_uboSlot = descriptorRequirements.AddDescriptor({
    DescriptorType::UniformBuffer, ShaderStage::Compute
    });

  const U32 UBO_SET   = descriptorRequirements.AddSet({UBO_SLOT});
  const U32 LIGHT_SET = descriptorRequirements.AddSet({LIGHT_SLOT});

  const U32 COMPUTE_UBO_SET = descriptorRequirements.AddSet({m_computePass.m_computeUBOSlot});
  const U32 COMPUTE_NORMALS_UBO_SET = descriptorRequirements.AddSet({m_normalsPass.m_uboSlot});

  ShaderRequirements shaderRequirements = ShaderRequirements(4, allocatorTemporary);
  const U32 COMPUTE_SHADER_ID           = shaderRequirements.AddShader({
    ShaderStage::Compute, "SolvingPass_Cloth.cs", AssetLocation::Shaders
  });

  const U32 VERTEX_SHADER_ID = shaderRequirements.AddShader({
    ShaderStage::Vertex, "ShadingPass_Cloth.vs", AssetLocation::Shaders
  });
  const U32 PIXEL_SHADER_ID = shaderRequirements.AddShader({
    ShaderStage::Pixel, "ShadingPass_Cloth.ps", AssetLocation::Shaders
  });

  const U32 COMPUTE_NORMALS_SHADER_ID           = shaderRequirements.AddShader({
    ShaderStage::Compute, "NormalsPass_Normalize.cs", AssetLocation::Shaders
    });

  const auto solvingView = m_clothPlane.GetPBDSolvingView(m_localAllocator);
  const auto& clothDistanceConstraints = solvingView.GetDistanceConstraints();
  const auto& clothBendingConstraints = solvingView.GetBendingConstraints();

  m_normalUBO = {};
  m_normalUBO.m_numTriangles = m_clothPlane.GetIndexCount() / 3;
  m_normalUBO.m_numVertices = m_clothPlane.GetVertexCount();

  RenderPassRequirements renderPassRequirements = RenderPassRequirements(0, 3, 20, allocatorTemporary);
  renderPassRequirements.m_maxPools             = 3;

  const U32 COMPUTE_VERTEX_BUFFER = renderPassRequirements.AddBuffer({
    U32(sizeof(Vector3f)) * U32(m_clothPlane.GetVertexCount()), U32(sizeof(Vector3f))
  });

  const U32 COMPUTE_PROJECTION_BUFFER = renderPassRequirements.AddBuffer({
    U32(sizeof(Vector3f)) * U32(m_clothPlane.GetVertexCount()), U32(sizeof(Vector3f))
  });

  const U32 COMPUTE_VERTEX_VELOCITY = renderPassRequirements.AddBuffer({
    U32(sizeof(Vector3f)) * U32(m_clothPlane.GetVertexCount()), U32(sizeof(Vector3f))
  });

  const U32 COMPUTE_VERTEX_INV_MASS = renderPassRequirements.AddBuffer({
    U32(sizeof(float)) * U32(m_clothPlane.GetVertexCount()), U32(sizeof(float))
    });

  const U32 COMPUTE_VERTEX_CONSTRAINT_COUNT = renderPassRequirements.AddBuffer({
    U32(sizeof(U32)) * U32(m_clothPlane.GetVertexCount()), U32(sizeof(U32))
  });

  const U32 COMPUTE_VERTEX_DELTAX = renderPassRequirements.AddBuffer({
    U32(sizeof(U32)) * U32(m_clothPlane.GetVertexCount()), U32(sizeof(U32))
  });

  const U32 COMPUTE_VERTEX_DELTAY = renderPassRequirements.AddBuffer({
    U32(sizeof(U32)) * U32(m_clothPlane.GetVertexCount()), U32(sizeof(U32))
  });

  const U32 COMPUTE_VERTEX_DELTAZ = renderPassRequirements.AddBuffer({
    U32(sizeof(U32)) * U32(m_clothPlane.GetVertexCount()), U32(sizeof(U32))
  });

  const U32 DISTANCE_CONSTRAINTS_BUFFER = renderPassRequirements.AddBuffer({
    U32(sizeof(DistanceConstraint)) * U32(clothDistanceConstraints.GetSize()), U32(sizeof(DistanceConstraint))
    });

  const U32 BEND_CONSTRAINTS_BUFFER = renderPassRequirements.AddBuffer({
    U32(sizeof(BendingConstraint)) * U32(clothBendingConstraints.GetSize()), U32(sizeof(BendingConstraint))
    });

  const U32 COMPUTE_VERTEX_NORMALS = renderPassRequirements.AddBuffer({
    U32(sizeof(Vector3f)) * U32(m_clothPlane.GetVertexCount()), U32(sizeof(Vector3f))
    });

  const U32 COMPUTE_VERTEX_NORMAL_X = renderPassRequirements.AddBuffer({
    U32(sizeof(U32)) * U32(m_clothPlane.GetVertexCount()), U32(sizeof(U32))
    });

  const U32 COMPUTE_VERTEX_NORMAL_Y = renderPassRequirements.AddBuffer({
    U32(sizeof(U32)) * U32(m_clothPlane.GetVertexCount()), U32(sizeof(U32))
    });

  const U32 COMPUTE_VERTEX_NORMAL_Z = renderPassRequirements.AddBuffer({
    U32(sizeof(U32)) * U32(m_clothPlane.GetVertexCount()), U32(sizeof(U32))
    });

  const U32 COMPUTE_INDEX_BUFFER = renderPassRequirements.AddBuffer({
    U32(sizeof(U32)) * U32(m_clothPlane.GetIndexCount()), U32(sizeof(U32))
    });

  m_computePass.m_passId = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{COMPUTE_SHADER_ID},
    PipelinePassCreateInfo::InputTargets{},
    PipelinePassCreateInfo::InputBuffers{
      {DISTANCE_CONSTRAINTS_BUFFER, ShaderStage::Compute}, {BEND_CONSTRAINTS_BUFFER, ShaderStage::Compute}, {COMPUTE_VERTEX_INV_MASS, ShaderStage::Compute}
    },
    PipelinePassCreateInfo::OutputTargets{},
    PipelinePassCreateInfo::OutputBuffers{
      COMPUTE_VERTEX_BUFFER, COMPUTE_PROJECTION_BUFFER, COMPUTE_VERTEX_VELOCITY, COMPUTE_VERTEX_CONSTRAINT_COUNT,
      COMPUTE_VERTEX_DELTAX, COMPUTE_VERTEX_DELTAY, COMPUTE_VERTEX_DELTAZ
    },
    PipelinePassCreateInfo::DescriptorSets{COMPUTE_UBO_SET},
    ClearData{{0.0f, 0.0f, 0.0f, 1.0f}, 1.0f, 0},
    BlendState{},
    RenderPassType::Compute
  });

  m_normalsPass.m_passId = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{COMPUTE_NORMALS_SHADER_ID},
    PipelinePassCreateInfo::InputTargets{},
    PipelinePassCreateInfo::InputBuffers{
      {COMPUTE_INDEX_BUFFER, ShaderStage::Compute}, {COMPUTE_VERTEX_BUFFER, ShaderStage::Compute}
    },
    PipelinePassCreateInfo::OutputTargets{},
    PipelinePassCreateInfo::OutputBuffers{
      COMPUTE_VERTEX_NORMALS, COMPUTE_VERTEX_NORMAL_X, COMPUTE_VERTEX_NORMAL_Y, COMPUTE_VERTEX_NORMAL_Z
    },
    PipelinePassCreateInfo::DescriptorSets{COMPUTE_NORMALS_UBO_SET},
    ClearData{{0.0f, 0.0f, 0.0f, 1.0f}, 1.0f, 0},
    BlendState{},
    RenderPassType::Compute
    });

  const U32 RENDER_PASS = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{VERTEX_SHADER_ID, PIXEL_SHADER_ID}, // SHADERS
    PipelinePassCreateInfo::InputTargets{},                             // INPUT TARGETS
    PipelinePassCreateInfo::InputBuffers{},                             // INPUT TARGETS
    PipelinePassCreateInfo::OutputTargets{},                            // OUTPUT TARGETS
    PipelinePassCreateInfo::OutputBuffers{},                            // OUTPUT TARGETS
    PipelinePassCreateInfo::DescriptorSets{UBO_SET, LIGHT_SET},         // DESCRIPTORS
    ClearData{{0.2f, 0.2f, 0.2f, 1.0f}, 1.0f, 0}
  });

  std::vector<Vector3f> zeroBufferData = std::vector<Vector3f>(m_clothPlane.GetVertexCount(), Vector3f(0.0f));
  std::vector<Vector3i> zeroIntVecData = std::vector<Vector3i>(m_clothPlane.GetVertexCount(), Vector3i(0));
  std::vector<U32> zeroIntBuffer       = std::vector<U32>(m_clothPlane.GetVertexCount(), 0);

  const auto totalConstraints = U32(clothDistanceConstraints.GetSize() + clothBendingConstraints.GetSize());

  m_renderer = RenderSystem::CreateRenderer(appInfo, requirements, applicationRequirements,
                                            m_window->GetSwapChainRequirements(), renderPassRequirements,
                                            descriptorRequirements, shaderRequirements, m_mainAllocator,
                                            m_drawableAllocator,
                                            *m_window);

  m_textureManager = RenderSystem::CreateTextureManager(textureRequirements);

  const auto clothParticleMass = reinterpret_cast<const U8*>(m_clothPlane.GetVertexInverseMass().Data()); // NOLINT
  m_renderer->BindBufferTarget(COMPUTE_VERTEX_INV_MASS, clothParticleMass);

  const auto clothDataStart = reinterpret_cast<const U8*>(m_clothPlane.VertexData()); // NOLINT
  m_renderer->BindBufferTarget(COMPUTE_VERTEX_BUFFER, clothDataStart);

  const auto projectionStart = reinterpret_cast<const U8*>(zeroBufferData.data()); // NOLINT
  m_renderer->BindBufferTarget(COMPUTE_PROJECTION_BUFFER, projectionStart);

  const auto distanceConstraintStart = reinterpret_cast<const U8*>(clothDistanceConstraints.Data()); // NOLINT
  const auto bendConstraintStart = reinterpret_cast<const U8*>(clothBendingConstraints.Data()); // NOLINT
  m_renderer->BindBufferTarget(DISTANCE_CONSTRAINTS_BUFFER, distanceConstraintStart);
  m_renderer->BindBufferTarget(BEND_CONSTRAINTS_BUFFER, bendConstraintStart);

  const auto velocityStart = reinterpret_cast<const U8*>(zeroBufferData.data()); // NOLINT
  m_renderer->BindBufferTarget(COMPUTE_VERTEX_VELOCITY, velocityStart);

  const auto bufferLock = reinterpret_cast<const U8*>(zeroIntBuffer.data()); // NOLINT
  m_renderer->BindBufferTarget(COMPUTE_VERTEX_CONSTRAINT_COUNT, bufferLock);

  const auto deltaStart = reinterpret_cast<const U8*>(zeroIntBuffer.data()); // NOLINT
  m_renderer->BindBufferTarget(COMPUTE_VERTEX_DELTAX, deltaStart);
  m_renderer->BindBufferTarget(COMPUTE_VERTEX_DELTAY, deltaStart);
  m_renderer->BindBufferTarget(COMPUTE_VERTEX_DELTAZ, deltaStart);

  m_renderer->BindBufferTarget(COMPUTE_INDEX_BUFFER, m_clothPlane.IndexData());


  // CREATE COMPUTE POOL - PBD
  const U32 numBlocks     = (totalConstraints + BLOCK_SIZE_X - 1) / BLOCK_SIZE_X;

  ComputePoolCreateInfo computePoolInfo = {allocatorTemporary};
  computePoolInfo.m_byteSize            = 0xF00000;
  computePoolInfo.m_computePasses       = {{m_computePass.m_passId}, allocatorTemporary};
  computePoolInfo.m_launchDims          = ThreadGroupDimensions{numBlocks, 1, 1};

  ComputePool& computePool = m_renderer->CreateComputePool(computePoolInfo);
  m_computePool            = &computePool;

  const float distanceStiffnessPrime = 1.0f - std::pow(1.0f - DISTANCE_STIFFNESS, 1.0f / SOLVER_ITERATIONS);
  const float bendingStiffnessPrime  = 1.0f - std::pow(1.0f - BENDING_STIFFNESS, 1.0f / SOLVER_ITERATIONS);

  m_computeUBO                         = {};
  m_computeUBO.m_numVertices           = m_clothPlane.GetVertexCount();
  m_computeUBO.m_stretchStiffness      = distanceStiffnessPrime;
  m_computeUBO.m_bendStiffness         = bendingStiffnessPrime;
  m_computeUBO.m_timeDelta             = 0.0f;
  m_computeUBO.m_numBlocks             = numBlocks;
  m_computeUBO.m_numStretchConstraints = U32(clothDistanceConstraints.GetSize());
  m_computeUBO.m_numBendConstraints    = U32(clothBendingConstraints.GetSize());

  const auto computeUBOStart = reinterpret_cast<const U8*>(&m_computeUBO); // NOLINT
  computePool.BindUniformData(m_computePass.m_computeUBOSlot, computeUBOStart, sizeof(ComputeUBO));

  // CREATE COMPUTE POOL - NORMALS
  const U32 numBlocksForNormalize     = (m_normalUBO.m_numVertices + BLOCK_SIZE_X - 1) / BLOCK_SIZE_X;

  ComputePoolCreateInfo normComputePoolInfo = {allocatorTemporary};
  normComputePoolInfo.m_byteSize            = 0xF00000;
  normComputePoolInfo.m_computePasses       = {{m_normalsPass.m_passId}, allocatorTemporary};
  normComputePoolInfo.m_launchDims          = ThreadGroupDimensions{numBlocksForNormalize, 1, 1};

  ComputePool& normComputePool = m_renderer->CreateComputePool(normComputePoolInfo);
  const auto normUBOStart = reinterpret_cast<const U8*>(&m_normalUBO); // NOLINT
  normComputePool.BindUniformData(m_normalsPass.m_uboSlot, normUBOStart, sizeof(NormalUBO));

  IcoSphere sphere(4);

  DrawablePoolCreateInfo poolInfo = {allocatorTemporary};
  poolInfo.m_byteSize             = sphere.TotalDataSize() + m_clothPlane.TotalDataSize() + 0x400000;
  poolInfo.m_numDrawables         = 2;
  poolInfo.m_renderPasses         = {{RENDER_PASS}, allocatorTemporary};
  poolInfo.m_drawType             = DrawType::InstancedIndexed;

  const auto VERTEX_SLOT = poolInfo.AddInputSlot({
    BufferUsageRate::PerVertex, {{"POSITION", m_clothPlane.GetVertexFormat()}}, 0, BufferSource::StructuredBuffer
  });

  const auto NORMAL_SLOT = poolInfo.AddInputSlot({
    BufferUsageRate::PerVertex, {{"NORMAL", m_clothPlane.GetNormalFormat()}}, 0, BufferSource::StructuredBuffer
  });

  DrawablePool& pool = m_renderer->CreateDrawablePool(poolInfo);

  const auto uboDataBuffer   = reinterpret_cast<U8*>(&m_clothUBO);  // NOLINT
  // const auto sphereUBO       = reinterpret_cast<U8*>(&m_sphereUBO); // NOLINT
  const auto lightDataBuffer = reinterpret_cast<U8*>(&lightData);   // NOLINT
  // Create Drawable from Pool
  DrawableCreateInfo createInfo = {};
  createInfo.m_vertexCount      = m_clothPlane.GetVertexCount();
  createInfo.m_indexCount       = m_clothPlane.GetIndexCount();
  createInfo.m_instanceCount    = 1;
  createInfo.m_indexType        = RawStorageFormat::R32_UINT;

  const auto clothId = pool.CreateDrawable(createInfo);
  pool.BindVertexData(clothId, VERTEX_SLOT, COMPUTE_VERTEX_BUFFER, 0, m_clothPlane.VertexDataSize());
  pool.BindVertexData(clothId, NORMAL_SLOT, COMPUTE_VERTEX_NORMALS, 0, m_clothPlane.NormalDataSize());
  pool.SetIndexData(clothId, m_clothPlane.IndexData(), m_clothPlane.IndexDataSize());
  pool.BindUniformData(clothId, UBO_SLOT, uboDataBuffer, sizeof(SceneUBO));
  pool.BindUniformData(clothId, LIGHT_SLOT, lightDataBuffer, sizeof(LightData));

  // DrawableCreateInfo sphereDrawableInfo = {};
  // sphereDrawableInfo.m_vertexCount      = sphere.GetVertexCount();
  // sphereDrawableInfo.m_indexCount       = sphere.GetIndexCount();
  // sphereDrawableInfo.m_instanceCount    = 1;
  // sphereDrawableInfo.m_indexType        = sphere.GetIndexFormat();
  //
  // DrawablePoolCreateInfo spherePoolInfo = {allocatorTemporary};
  // spherePoolInfo.m_byteSize             = sphere.TotalDataSize() + 0x400000;
  // spherePoolInfo.m_numDrawables         = 1;
  // spherePoolInfo.m_renderPasses         = {{RENDER_PASS}, allocatorTemporary};
  // spherePoolInfo.m_drawType             = DrawType::InstancedIndexed;
  //
  // const auto SPHERE_VERTEX_SLOT = spherePoolInfo.AddInputSlot({
  //   BufferUsageRate::PerVertex, {{"POSITION", RawStorageFormat::R32G32B32A32_FLOAT}}
  // });
  //
  // const auto SPHERE_NORMAL_SLOT = spherePoolInfo.AddInputSlot({
  //   BufferUsageRate::PerVertex, {{"NORMAL", RawStorageFormat::R32G32B32_FLOAT}}
  // });

  // DrawablePool& spherePool = m_renderer->CreateDrawablePool(spherePoolInfo);
  //
  // const auto sphereId = spherePool.CreateDrawable(sphereDrawableInfo);
  // spherePool.BindVertexData(sphereId, SPHERE_VERTEX_SLOT, sphere.VertexData(), sphere.VertexDataSize());
  // spherePool.BindVertexData(sphereId, SPHERE_NORMAL_SLOT, sphere.NormalData(), sphere.NormalDataSize());
  // spherePool.SetIndexData(sphereId, sphere.IndexData(), sphere.IndexDataSize());
  // spherePool.BindUniformData(sphereId, UBO_SLOT, sphereUBO, sizeof(SceneUBO));
  // spherePool.BindUniformData(sphereId, LIGHT_SLOT, lightDataBuffer, sizeof(LightData));

  m_renderPass.m_vertexSlot   = VERTEX_SLOT;
  m_renderPass.m_normalSlot   = NORMAL_SLOT;
  m_renderPass.m_sceneUBOSlot = UBO_SLOT;
  m_renderPass.m_clothId      = clothId;
  // m_renderPass.m_sphereId     = sphereId;

  m_mainPool   = &pool;
  // m_spherePool = &spherePool;

  // All Drawables Done
  m_renderer->Submit();

  LOG_INF(log_AppRenderer, LOG_LEVEL, "Initialized AppRenderer");
}

void AppRenderer::WindowUpdate(float timeDelta) {
  if (timeDelta < EPSILON) {
    return;
  }

  m_camera.Update(timeDelta);

  // timeDelta = 0.0166667f;

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

  const auto uboDataBuffer    = reinterpret_cast<U8*>(&m_clothUBO);         // NOLINT
  // const auto sphereDataBuffer = reinterpret_cast<U8*>(&m_sphereUBO);        // NOLINT
  const auto computeUBOStart  = reinterpret_cast<const U8*>(&m_computeUBO); // NOLINT

  m_computePool->BeginUpdates();
  m_computePool->UpdateUniformData(m_computePass.m_computeUBOSlot, computeUBOStart, sizeof(ComputeUBO));
  m_computePool->SubmitUpdates();

  m_mainPool->BeginUpdates();
  // Update Cloth
  m_mainPool->UpdateUniformData(m_renderPass.m_clothId, m_renderPass.m_sceneUBOSlot, uboDataBuffer, sizeof(SceneUBO));
  // Update Sphere
  m_mainPool->SubmitUpdates();

  // m_spherePool->BeginUpdates();
  // m_spherePool->UpdateUniformData(m_renderPass.m_sphereId, m_renderPass.m_sceneUBOSlot, sphereDataBuffer,
  //                                 sizeof(SceneUBO));
  // m_spherePool->SubmitUpdates();

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
