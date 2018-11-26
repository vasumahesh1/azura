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

const Vector3f TEST_SPHERE_CENTER = Vector3f(0, -5, 0);
const Vector3f TEST_PLANE_CENTER = Vector3f(0, -8.01f, 0);
constexpr float TEST_SPHERE_RADIUS = 2.9f;
constexpr float CLOTH_SPAN = 5;
constexpr U32 CLOTH_DIV_X = 30;
constexpr U32 CLOTH_DIV_Y = 30;
constexpr U32 TEXTURE_MEMORY = 0x4000000; // 64 MB
constexpr U32 ANCHOR_IDX_1 = 0;
constexpr U32 ANCHOR_IDX_2 = (CLOTH_DIV_Y * (CLOTH_DIV_X + 1));

} // namespace

AppRenderer::AppRenderer()
  : m_mainBuffer(0xC00'0000),
    m_mainAllocator(m_mainBuffer, 0x400'0000),
    m_localAllocator(m_mainBuffer, 0x400'0000),
    m_drawableAllocator(m_mainBuffer, 0x400'0000),
    m_camera(1280, 720),
    m_clothPlane(ClothTriangulation::Regular, Vector2f(-CLOTH_SPAN, -CLOTH_SPAN), Vector2f(CLOTH_SPAN, CLOTH_SPAN),
                 Vector2u(CLOTH_DIV_X, CLOTH_DIV_Y), m_localAllocator),
    log_AppRenderer(Log("AppRenderer")) {
}

void AppRenderer::Initialize() {
  LOG_INF(log_AppRenderer, LOG_LEVEL, "Starting Init of AppRenderer");

  m_clothTransform.SetForwardKey(KeyboardKey::Up);
  m_clothTransform.SetBackwardKey(KeyboardKey::Down);
  m_clothTransform.SetLeftKey(KeyboardKey::Left);
  m_clothTransform.SetRightKey(KeyboardKey::Right);
  m_clothTransform.SetStepSize(10.0f);

  HEAP_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 16384);
  m_window = RenderSystem::CreateApplicationWindow("PBD Cloth Simulation", 1280, 720);

  m_window->SetUpdateCallback([this](float deltaTime)
  {
    WindowUpdate(deltaTime);
  });

  m_window->SetKeyEventCallback([this](KeyEvent evt)
  {
    m_camera.OnKeyEvent(evt);
    m_clothTransform.OnKeyEvent(evt);
  });

  m_window->SetMouseEventCallback([this](MouseEvent evt)
  {
    UNUSED(evt);
    // Disable as it has bugs
    // m_camera.OnMouseEvent(e);
  });

  m_camera.SetZoom(35);
  m_camera.RotateAboutRight(10);
  m_camera.SetStepSize(30.0f);

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
  m_clothUBO.m_model             = m_clothTransform.GetTransform();
  m_clothUBO.m_view              = m_camera.GetViewMatrix();
  m_clothUBO.m_viewProj          = m_camera.GetViewProjMatrix();
  m_clothUBO.m_invViewProj       = m_camera.GetInvViewProjMatrix();
  m_clothUBO.m_invProj           = m_camera.GetProjMatrix().Inverse();
  m_clothUBO.m_modelInvTranspose = m_clothUBO.m_model.Inverse().Transpose();

  m_sphereUBO                     = {};
  m_sphereUBO.m_model             = Matrix4f::FromTranslationVector(TEST_SPHERE_CENTER) * Matrix4f::FromScaleVector(Vector3f(TEST_SPHERE_RADIUS, TEST_SPHERE_RADIUS, TEST_SPHERE_RADIUS));
  m_sphereUBO.m_viewProj          = m_camera.GetViewProjMatrix();
  m_sphereUBO.m_view              = m_camera.GetViewMatrix();
  m_sphereUBO.m_invViewProj       = m_camera.GetInvViewProjMatrix();
  m_sphereUBO.m_invProj           = m_camera.GetProjMatrix().Inverse();
  m_sphereUBO.m_modelInvTranspose = m_sphereUBO.m_model.Inverse().Transpose();

  m_planeUBO = {};
  m_planeUBO.m_model = Matrix4f::FromTranslationVector(TEST_PLANE_CENTER);
  m_planeUBO.m_viewProj = m_camera.GetViewProjMatrix();
  m_planeUBO.m_view = m_camera.GetViewMatrix();
  m_planeUBO.m_invViewProj = m_camera.GetInvViewProjMatrix();
  m_planeUBO.m_invProj = m_camera.GetProjMatrix().Inverse();
  m_planeUBO.m_modelInvTranspose = m_planeUBO.m_model.Inverse().Transpose();

  LightData lightData  = {};
  lightData.m_lightPos = Vector4f(0.0f, 15.0f, 20.0f, 1.0f);

  // TODO(vasumahesh1):[Q]:Allocator?
  const ApplicationRequirements applicationRequirements = {};

  TextureRequirements textureRequirements = {};
  textureRequirements.m_maxCount          = 15;
  textureRequirements.m_poolSize          = TEXTURE_MEMORY;

  DescriptorRequirements descriptorRequirements = DescriptorRequirements(10, 6, allocatorTemporary);
  // SET 0
  const U32 UBO_SLOT = descriptorRequirements.AddDescriptor({
    DescriptorType::UniformBuffer, ShaderStage::Vertex
  });
  const U32 LIGHT_SLOT = descriptorRequirements.AddDescriptor({
    DescriptorType::UniformBuffer, ShaderStage::Vertex
  });

  const U32 SAMPLER_SLOT = descriptorRequirements.AddDescriptor({ DescriptorType::Sampler, ShaderStage::Pixel });
  const U32 TEXTURE_SLOT = descriptorRequirements.AddDescriptor({ DescriptorType::SampledImage, ShaderStage::Pixel });
  const U32 NORMALS_SLOT = descriptorRequirements.AddDescriptor({ DescriptorType::SampledImage, ShaderStage::Pixel });
  const U32 ROUGHNESS_SLOT = descriptorRequirements.AddDescriptor({ DescriptorType::SampledImage, ShaderStage::Pixel });
  const U32 AO_SLOT = descriptorRequirements.AddDescriptor({ DescriptorType::SampledImage, ShaderStage::Pixel });

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
  const U32 SAMPLER_SET = descriptorRequirements.AddSet({SAMPLER_SLOT});
  const U32 TEXTURE_SET = descriptorRequirements.AddSet({TEXTURE_SLOT, NORMALS_SLOT, ROUGHNESS_SLOT, AO_SLOT});

  const U32 COMPUTE_UBO_SET = descriptorRequirements.AddSet({m_computePass.m_computeUBOSlot});
  const U32 COMPUTE_NORMALS_UBO_SET = descriptorRequirements.AddSet({m_normalsPass.m_uboSlot});

  ShaderRequirements shaderRequirements = ShaderRequirements(15, allocatorTemporary);
  // const U32 MAIN_COMPUTE_SHADER_ID           = shaderRequirements.AddShader({
  //   ShaderStage::Compute, "SolvingPass_Cloth.cs", AssetLocation::Shaders
  // });

  const U32 COMPUTE_1_PBD           = shaderRequirements.AddShader({
      ShaderStage::Compute, "SolvingPass_Cloth_ComputeProjectedPositions.cs", AssetLocation::Shaders
    });

  const U32 COMPUTE_2_PBD           = shaderRequirements.AddShader({
    ShaderStage::Compute, "SolvingPass_Cloth_ApplyConstraints.cs", AssetLocation::Shaders
    });

  const U32 COMPUTE_3_PBD           = shaderRequirements.AddShader({
    ShaderStage::Compute, "SolvingPass_Cloth_ApplyDelta.cs", AssetLocation::Shaders
    });

  const U32 COMPUTE_4_PBD           = shaderRequirements.AddShader({
    ShaderStage::Compute, "SolvingPass_Cloth_ComputePositions.cs", AssetLocation::Shaders
    });

  const U32 VERTEX_SHADER_ID = shaderRequirements.AddShader({
    ShaderStage::Vertex, "ShadingPass_Cloth.vs", AssetLocation::Shaders
  });
  const U32 PIXEL_SHADER_ID = shaderRequirements.AddShader({
    ShaderStage::Pixel, "ShadingPass_Cloth.ps", AssetLocation::Shaders
  });

  const U32 SPHERE_VERTEX_SHADER_ID = shaderRequirements.AddShader({
    ShaderStage::Vertex, "ShadingPass_Sphere.vs", AssetLocation::Shaders
    });
  const U32 SPHERE_PIXEL_SHADER_ID = shaderRequirements.AddShader({
    ShaderStage::Pixel, "ShadingPass_Sphere.ps", AssetLocation::Shaders
    });

  const U32 PLANE_VERTEX_SHADER_ID = shaderRequirements.AddShader({
    ShaderStage::Vertex, "ShadingPass_Plane.vs", AssetLocation::Shaders
    });
  const U32 PLANE_PIXEL_SHADER_ID = shaderRequirements.AddShader({
    ShaderStage::Pixel, "ShadingPass_Plane.ps", AssetLocation::Shaders
    });

  const U32 COMPUTE_NORMALS_SHADER_ID           = shaderRequirements.AddShader({
    ShaderStage::Compute, "NormalsPass_Normalize.cs", AssetLocation::Shaders
    });

  m_clothPlane.SetAnchorOnIndex(ANCHOR_IDX_1);
  m_clothPlane.SetAnchorOnIndex(ANCHOR_IDX_2);

  const auto solvingView = m_clothPlane.GetPBDSolvingView(m_localAllocator);
  const auto& clothDistanceConstraints = solvingView.GetDistanceConstraints();
  const auto& clothBendingConstraints = solvingView.GetBendingConstraints();

  m_normalUBO = {};
  m_normalUBO.m_numTriangles = m_clothPlane.GetIndexCount() / 3;
  m_normalUBO.m_numVertices = m_clothPlane.GetVertexCount();

  RenderPassRequirements renderPassRequirements = RenderPassRequirements(0, 10, 20, allocatorTemporary);
  renderPassRequirements.m_maxPools             = 10;

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

  const U32 COMPUTE_INDEX_BUFFER = renderPassRequirements.AddBuffer({
    U32(sizeof(U32)) * U32(m_clothPlane.GetIndexCount()), U32(sizeof(U32))
    });
  
  const U32 COMPUTE_VERTEX_TANX = renderPassRequirements.AddBuffer({
    U32(sizeof(U32)) * U32(m_clothPlane.GetVertexCount()), U32(sizeof(U32))
    });
  
  const U32 COMPUTE_VERTEX_TANY = renderPassRequirements.AddBuffer({
    U32(sizeof(U32)) * U32(m_clothPlane.GetVertexCount()), U32(sizeof(U32))
    });
  
  const U32 COMPUTE_VERTEX_TANZ = renderPassRequirements.AddBuffer({
    U32(sizeof(U32)) * U32(m_clothPlane.GetVertexCount()), U32(sizeof(U32))
    });

  m_computePass.m_pass1 = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{},
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

  m_computePass.m_pass2 = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{},
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

  m_computePass.m_pass3 = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{},
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

  m_computePass.m_pass4 = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{},
    PipelinePassCreateInfo::InputTargets{},
    PipelinePassCreateInfo::InputBuffers{
      {DISTANCE_CONSTRAINTS_BUFFER, ShaderStage::Compute}, {BEND_CONSTRAINTS_BUFFER, ShaderStage::Compute}, {COMPUTE_VERTEX_INV_MASS, ShaderStage::Compute}
    },
    PipelinePassCreateInfo::OutputTargets{},
    PipelinePassCreateInfo::OutputBuffers{
      COMPUTE_VERTEX_BUFFER, COMPUTE_PROJECTION_BUFFER, COMPUTE_VERTEX_VELOCITY, COMPUTE_VERTEX_CONSTRAINT_COUNT,
      COMPUTE_VERTEX_DELTAX, COMPUTE_VERTEX_DELTAY, COMPUTE_VERTEX_DELTAZ, COMPUTE_VERTEX_TANX, COMPUTE_VERTEX_TANY, COMPUTE_VERTEX_TANZ
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
      COMPUTE_VERTEX_DELTAX, COMPUTE_VERTEX_DELTAY, COMPUTE_VERTEX_DELTAZ, COMPUTE_VERTEX_TANX, COMPUTE_VERTEX_TANY, COMPUTE_VERTEX_TANZ
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
    PipelinePassCreateInfo::DescriptorSets{UBO_SET, LIGHT_SET, SAMPLER_SET, TEXTURE_SET},         // DESCRIPTORS
    ClearData{{0.1f, 0.1f, 0.1f, 1.0f}, 1.0f, 0}
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

  const U32 albedoTexture = m_textureManager->Load("Textures/Fabric10_col.jpg");
  const TextureDesc* albedoDesc = m_textureManager->GetInfo(albedoTexture);
  VERIFY_TRUE(log_AppRenderer, albedoDesc != nullptr, "albedoDesc was Null");

  const U32 normalTexture = m_textureManager->Load("Textures/Fabric10_nrm.jpg");
  const TextureDesc* normalDesc = m_textureManager->GetInfo(normalTexture);
  VERIFY_TRUE(log_AppRenderer, normalDesc != nullptr, "normalDesc was Null");

  const U32 roughnessTexture = m_textureManager->Load("Textures/Fabric10_rgh.jpg");
  const TextureDesc* roughnessDesc = m_textureManager->GetInfo(roughnessTexture);
  VERIFY_TRUE(log_AppRenderer, roughnessDesc != nullptr, "roughnessDesc was Null");

  const U32 aoTexture = m_textureManager->Load("Textures/Fabric10_AO.jpg");
  const TextureDesc* aoDesc = m_textureManager->GetInfo(aoTexture);
  VERIFY_TRUE(log_AppRenderer, aoDesc != nullptr, "aoDesc was Null");

  const U32 floorAlbedo = m_textureManager->Load("Textures/PavingStones35_col.jpg");
  const TextureDesc* floorAlbedoDesc = m_textureManager->GetInfo(floorAlbedo);
  VERIFY_TRUE(log_AppRenderer, floorAlbedoDesc != nullptr, "floorAlbedoDesc was Null");

  const U32 floorNormal = m_textureManager->Load("Textures/PavingStones35_nrm.jpg");
  const TextureDesc* floorNormalDesc = m_textureManager->GetInfo(floorNormal);
  VERIFY_TRUE(log_AppRenderer, floorNormalDesc != nullptr, "floorNormalDesc was Null");

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
  const U32 numConstraintBlocks     = (totalConstraints + DEFAULT_BLOCK_SIZE_X - 1) / DEFAULT_BLOCK_SIZE_X;
  const U32 numVerticesBlocks     = (m_clothPlane.GetVertexCount() + DEFAULT_BLOCK_SIZE_X - 1) / DEFAULT_BLOCK_SIZE_X;

  ComputePoolCreateInfo poolInfoConstraints = {allocatorTemporary};
  poolInfoConstraints.m_byteSize            = 0xF00000;
  poolInfoConstraints.m_computePasses       = {{m_computePass.m_pass2}, allocatorTemporary};
  poolInfoConstraints.m_launchDims          = ThreadGroupDimensions{numConstraintBlocks, 1, 1};

  ComputePoolCreateInfo pass1 = {allocatorTemporary};
  pass1.m_byteSize            = 0xF00000;
  pass1.m_computePasses       = {{m_computePass.m_pass1}, allocatorTemporary};
  pass1.m_launchDims          = ThreadGroupDimensions{numVerticesBlocks, 1, 1};

  ComputePoolCreateInfo pass2 = {allocatorTemporary};
  pass2.m_byteSize            = 0xF00000;
  pass2.m_computePasses       = {{m_computePass.m_pass3}, allocatorTemporary};
  pass2.m_launchDims          = ThreadGroupDimensions{numVerticesBlocks, 1, 1};

  ComputePoolCreateInfo pass3 = {allocatorTemporary};
  pass3.m_byteSize            = 0xF00000;
  pass3.m_computePasses       = {{m_computePass.m_pass4}, allocatorTemporary};
  pass3.m_launchDims          = ThreadGroupDimensions{numVerticesBlocks, 1, 1};

  const float distanceStiffnessPrime = 1.0f - std::pow(1.0f - DISTANCE_STIFFNESS, 1.0f / SOLVER_ITERATIONS);
  const float bendingStiffnessPrime  = 1.0f - std::pow(1.0f - BENDING_STIFFNESS, 1.0f / SOLVER_ITERATIONS);

  m_computeUBO                         = {};
  m_computeUBO.m_numVertices           = m_clothPlane.GetVertexCount();
  m_computeUBO.m_stretchStiffness      = distanceStiffnessPrime;
  m_computeUBO.m_bendStiffness         = bendingStiffnessPrime;
  m_computeUBO.m_timeDelta             = 0.0f;
  m_computeUBO.m_numBlocks             = numConstraintBlocks;
  m_computeUBO.m_numStretchConstraints = U32(clothDistanceConstraints.GetSize());
  m_computeUBO.m_numBendConstraints    = U32(clothBendingConstraints.GetSize());
  m_computeUBO.m_clothModelMatrix      = m_clothTransform.GetTransform().Transpose();
  const auto computeUBOStart = reinterpret_cast<const U8*>(&m_computeUBO); // NOLINT

  ComputePool& computeProjectedPositions = m_renderer->CreateComputePool(pass1);
  computeProjectedPositions.AddShader(COMPUTE_1_PBD);
  computeProjectedPositions.BindUniformData(m_computePass.m_computeUBOSlot, computeUBOStart, sizeof(ComputeUBO));
  m_computePools[0] = &computeProjectedPositions;

  ComputePool& computeApplyConstraints = m_renderer->CreateComputePool(poolInfoConstraints);
  computeApplyConstraints.AddShader(COMPUTE_2_PBD);
  computeApplyConstraints.BindUniformData(m_computePass.m_computeUBOSlot, computeUBOStart, sizeof(ComputeUBO));
  m_computePools[1] = &computeApplyConstraints;

  ComputePool& computeApplyDelta = m_renderer->CreateComputePool(pass2);
  computeApplyDelta.AddShader(COMPUTE_3_PBD);
  computeApplyDelta.BindUniformData(m_computePass.m_computeUBOSlot, computeUBOStart, sizeof(ComputeUBO));
  m_computePools[2] = &computeApplyDelta;

  ComputePool& computeComputePositions = m_renderer->CreateComputePool(pass3);
  computeComputePositions.AddShader(COMPUTE_4_PBD);
  computeComputePositions.BindUniformData(m_computePass.m_computeUBOSlot, computeUBOStart, sizeof(ComputeUBO));
  m_computePools[3] = &computeComputePositions;

  // CREATE COMPUTE POOL - NORMALS
  const U32 numBlocksForNormalize     = (m_normalUBO.m_numTriangles + DEFAULT_BLOCK_SIZE_X - 1) / DEFAULT_BLOCK_SIZE_X;

  ComputePoolCreateInfo normComputePoolInfo = {allocatorTemporary};
  normComputePoolInfo.m_byteSize            = 0xF00000;
  normComputePoolInfo.m_computePasses       = {{m_normalsPass.m_passId}, allocatorTemporary};
  normComputePoolInfo.m_launchDims          = ThreadGroupDimensions{numBlocksForNormalize, 1, 1};

  ComputePool& normComputePool = m_renderer->CreateComputePool(normComputePoolInfo);
  const auto normUBOStart = reinterpret_cast<const U8*>(&m_normalUBO); // NOLINT
  normComputePool.BindUniformData(m_normalsPass.m_uboSlot, normUBOStart, sizeof(NormalUBO));

  IcoSphere sphere(4);
  Plane plane(Vector2f(-150, -150), Vector2f(150, 150), Vector2u(10, 10), Vector2u(10, 10));

  DrawablePoolCreateInfo poolInfo = {allocatorTemporary};
  poolInfo.m_byteSize             = sphere.TotalDataSize() + m_clothPlane.TotalDataSize() + TEXTURE_MEMORY + 0x400000;
  poolInfo.m_numDrawables         = 2;
  poolInfo.m_renderPasses         = {{RENDER_PASS}, allocatorTemporary};
  poolInfo.m_drawType             = DrawType::InstancedIndexed;

  const auto VERTEX_SLOT = poolInfo.AddInputSlot({
    BufferUsageRate::PerVertex, {{"POSITION", m_clothPlane.GetVertexFormat()}}, 0, BufferSource::StructuredBuffer
  });

  const auto UV_SLOT = poolInfo.AddInputSlot({
    BufferUsageRate::PerVertex, {{"UV", m_clothPlane.GetUVFormat()}}
    });

  const auto NORMAL_SLOT_X = poolInfo.AddInputSlot({
    BufferUsageRate::PerVertex, {{"NORMALX", RawStorageFormat::R32_UINT}}, 0, BufferSource::StructuredBuffer
  });
  
  const auto NORMAL_SLOT_Y = poolInfo.AddInputSlot({
    BufferUsageRate::PerVertex, {{"NORMALY", RawStorageFormat::R32_UINT}}, 0, BufferSource::StructuredBuffer
  });
  
  const auto NORMAL_SLOT_Z = poolInfo.AddInputSlot({
    BufferUsageRate::PerVertex, {{"NORMALZ", RawStorageFormat::R32_UINT}}, 0, BufferSource::StructuredBuffer
  });

  const auto TANGENT_SLOT_X = poolInfo.AddInputSlot({
    BufferUsageRate::PerVertex, {{"TANGENTX", RawStorageFormat::R32_UINT}}, 0, BufferSource::StructuredBuffer
    });
  
  const auto TANGENT_SLOT_Y = poolInfo.AddInputSlot({
    BufferUsageRate::PerVertex, {{"TANGENTY", RawStorageFormat::R32_UINT}}, 0, BufferSource::StructuredBuffer
    });
  
  const auto TANGENT_SLOT_Z = poolInfo.AddInputSlot({
    BufferUsageRate::PerVertex, {{"TANGENTZ", RawStorageFormat::R32_UINT}}, 0, BufferSource::StructuredBuffer
    });

  DrawablePool& clothPool = m_renderer->CreateDrawablePool(poolInfo);

  const auto uboDataBuffer   = reinterpret_cast<U8*>(&m_clothUBO);  // NOLINT
  const auto sphereUBO       = reinterpret_cast<U8*>(&m_sphereUBO); // NOLINT
  const auto planeUBO        = reinterpret_cast<U8*>(&m_planeUBO); // NOLINT
  const auto lightDataBuffer = reinterpret_cast<U8*>(&lightData);   // NOLINT
  // Create Drawable from Pool
  DrawableCreateInfo createInfo = {};
  createInfo.m_vertexCount      = m_clothPlane.GetVertexCount();
  createInfo.m_indexCount       = m_clothPlane.GetIndexCount();
  createInfo.m_instanceCount    = 1;
  createInfo.m_indexType        = RawStorageFormat::R32_UINT;

  SamplerDesc desc = {};
  desc.m_filter = TextureFilter::MinMagMipLinear;
  desc.m_addressModeU = TextureAddressMode::Mirror;
  desc.m_addressModeV = TextureAddressMode::Mirror;
  desc.m_addressModeW = TextureAddressMode::Mirror;
  clothPool.BindSampler(SAMPLER_SLOT, desc);
  clothPool.BindTextureData(TEXTURE_SLOT, *albedoDesc, m_textureManager->GetData(albedoTexture));
  clothPool.BindTextureData(NORMALS_SLOT, *normalDesc, m_textureManager->GetData(normalTexture));
  clothPool.BindTextureData(ROUGHNESS_SLOT, *roughnessDesc, m_textureManager->GetData(roughnessTexture));
  clothPool.BindTextureData(AO_SLOT, *aoDesc, m_textureManager->GetData(aoTexture));

  const auto clothId = clothPool.CreateDrawable(createInfo);
  clothPool.BindVertexData(clothId, VERTEX_SLOT, COMPUTE_VERTEX_BUFFER, 0, m_clothPlane.VertexDataSize());
  clothPool.BindVertexData(clothId, UV_SLOT, m_clothPlane.UVData(), m_clothPlane.UVDataSize());
  clothPool.BindVertexData(clothId, NORMAL_SLOT_X, COMPUTE_VERTEX_DELTAX, 0, m_clothPlane.GetVertexCount() * U32(sizeof(float)));
  clothPool.BindVertexData(clothId, NORMAL_SLOT_Y, COMPUTE_VERTEX_DELTAY, 0, m_clothPlane.GetVertexCount() * U32(sizeof(float)));
  clothPool.BindVertexData(clothId, NORMAL_SLOT_Z, COMPUTE_VERTEX_DELTAZ, 0, m_clothPlane.GetVertexCount() * U32(sizeof(float)));
  clothPool.BindVertexData(clothId, TANGENT_SLOT_X, COMPUTE_VERTEX_TANX, 0, m_clothPlane.GetVertexCount() * U32(sizeof(float)));
  clothPool.BindVertexData(clothId, TANGENT_SLOT_Y, COMPUTE_VERTEX_TANY, 0, m_clothPlane.GetVertexCount() * U32(sizeof(float)));
  clothPool.BindVertexData(clothId, TANGENT_SLOT_Z, COMPUTE_VERTEX_TANZ, 0, m_clothPlane.GetVertexCount() * U32(sizeof(float)));
  clothPool.SetIndexData(clothId, m_clothPlane.IndexData(), m_clothPlane.IndexDataSize());
  clothPool.BindUniformData(clothId, UBO_SLOT, uboDataBuffer, sizeof(SceneUBO));
  clothPool.BindUniformData(clothId, LIGHT_SLOT, lightDataBuffer, sizeof(LightData));

   DrawableCreateInfo sphereDrawableInfo = {};
   sphereDrawableInfo.m_vertexCount      = sphere.GetVertexCount();
   sphereDrawableInfo.m_indexCount       = sphere.GetIndexCount();
   sphereDrawableInfo.m_instanceCount    = 1;
   sphereDrawableInfo.m_indexType        = sphere.GetIndexFormat();

   DrawableCreateInfo planeDrawableInfo = {};
   planeDrawableInfo.m_vertexCount = plane.GetVertexCount();
   planeDrawableInfo.m_indexCount = plane.GetIndexCount();
   planeDrawableInfo.m_instanceCount = 1;
   planeDrawableInfo.m_indexType = plane.GetIndexFormat();
  
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
   spherePool.AddShader(SPHERE_VERTEX_SHADER_ID);
   spherePool.AddShader(SPHERE_PIXEL_SHADER_ID);
  
   const auto sphereId = spherePool.CreateDrawable(sphereDrawableInfo);
   spherePool.BindVertexData(sphereId, SPHERE_VERTEX_SLOT, sphere.VertexData(), sphere.VertexDataSize());
   spherePool.BindVertexData(sphereId, SPHERE_NORMAL_SLOT, sphere.NormalData(), sphere.NormalDataSize());
   spherePool.SetIndexData(sphereId, sphere.IndexData(), sphere.IndexDataSize());
   spherePool.BindUniformData(sphereId, UBO_SLOT, sphereUBO, sizeof(SceneUBO));
   spherePool.BindUniformData(sphereId, LIGHT_SLOT, lightDataBuffer, sizeof(LightData));

   DrawablePoolCreateInfo planePoolInfo = {allocatorTemporary};
   planePoolInfo.m_byteSize             = plane.TotalDataSize() + 0x400000;
   planePoolInfo.m_numDrawables         = 1;
   planePoolInfo.m_renderPasses         = {{RENDER_PASS}, allocatorTemporary};
   planePoolInfo.m_drawType             = DrawType::InstancedIndexed;

   const auto PLANE_VERTEX_SLOT = planePoolInfo.AddInputSlot({
     BufferUsageRate::PerVertex, {{"POSITION", plane.GetVertexFormat()}}
     });

   const auto PLANE_NORMAL_SLOT = planePoolInfo.AddInputSlot({
     BufferUsageRate::PerVertex, {{"NORMAL", plane.GetNormalFormat()}}
     });

   const auto PLANE_UV_SLOT = planePoolInfo.AddInputSlot({
     BufferUsageRate::PerVertex, {{"UV", plane.GetUVFormat()}}
     });

   DrawablePool& planePool = m_renderer->CreateDrawablePool(planePoolInfo);
   planePool.AddShader(PLANE_VERTEX_SHADER_ID);
   planePool.AddShader(PLANE_PIXEL_SHADER_ID);

   SamplerDesc floorSampler = {};
   floorSampler.m_filter = TextureFilter::MinMagMipLinear;
   floorSampler.m_addressModeU = TextureAddressMode::Wrap;
   floorSampler.m_addressModeV = TextureAddressMode::Wrap;
   floorSampler.m_addressModeW = TextureAddressMode::Wrap;
   planePool.BindSampler(SAMPLER_SLOT, floorSampler);
   planePool.BindTextureData(TEXTURE_SLOT, *floorAlbedoDesc, m_textureManager->GetData(floorAlbedo));
   planePool.BindTextureData(NORMALS_SLOT, *floorNormalDesc, m_textureManager->GetData(floorNormal));

   const auto planeId = planePool.CreateDrawable(planeDrawableInfo);
   planePool.BindVertexData(planeId, PLANE_VERTEX_SLOT, plane.VertexData(), plane.VertexDataSize());
   planePool.BindVertexData(planeId, PLANE_NORMAL_SLOT, plane.NormalData(), plane.NormalDataSize());
   planePool.BindVertexData(planeId, PLANE_UV_SLOT, plane.UVData(), plane.UVDataSize());
   planePool.SetIndexData(planeId, plane.IndexData(), plane.IndexDataSize());
   planePool.BindUniformData(planeId, UBO_SLOT, planeUBO, sizeof(SceneUBO));
   planePool.BindUniformData(planeId, LIGHT_SLOT, lightDataBuffer, sizeof(LightData));

  m_renderPass.m_vertexSlot   = VERTEX_SLOT;
  m_renderPass.m_sceneUBOSlot = UBO_SLOT;
  m_renderPass.m_clothId      = clothId;
  m_renderPass.m_sphereId     = sphereId;
  m_renderPass.m_planeId      = planeId;

  m_mainPool   = &clothPool;
  m_spherePool = &spherePool;
  m_planePool = &planePool;

  // All Drawables Done
  m_renderer->Submit();

  LOG_INF(log_AppRenderer, LOG_LEVEL, "Initialized AppRenderer");
}

void AppRenderer::WindowUpdate(float timeDelta) {
  if (timeDelta < EPSILON) {
    return;
  }

  m_camera.Update(timeDelta);
  m_clothTransform.Update(timeDelta);

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

  m_planeUBO.m_view = m_camera.GetViewMatrix();
  m_planeUBO.m_viewProj = m_camera.GetViewProjMatrix();
  m_planeUBO.m_invViewProj = m_camera.GetInvViewProjMatrix();
  m_planeUBO.m_invProj = m_camera.GetProjMatrix().Inverse();
  m_planeUBO.m_modelInvTranspose = m_planeUBO.m_model.Inverse().Transpose();

  m_computeUBO.m_timeDelta = timeDelta;
  m_computeUBO.m_clothModelMatrix      = m_clothTransform.GetTransform();

  const auto uboDataBuffer    = reinterpret_cast<U8*>(&m_clothUBO);         // NOLINT
  const auto sphereDataBuffer = reinterpret_cast<U8*>(&m_sphereUBO);        // NOLINT
  const auto planeDataBuffer = reinterpret_cast<U8*>(&m_planeUBO);        // NOLINT
  const auto computeUBOStart  = reinterpret_cast<const U8*>(&m_computeUBO); // NOLINT

  for (auto& m_computePool : m_computePools) {
    m_computePool->BeginUpdates();
    m_computePool->UpdateUniformData(m_computePass.m_computeUBOSlot, computeUBOStart, sizeof(ComputeUBO));
    m_computePool->SubmitUpdates();
  }

  // m_computePool->BeginUpdates();
  // m_computePool->UpdateUniformData(m_computePass.m_computeUBOSlot, computeUBOStart, sizeof(ComputeUBO));
  // m_computePool->SubmitUpdates();

  m_mainPool->BeginUpdates();
  // Update Cloth
  m_mainPool->UpdateUniformData(m_renderPass.m_clothId, m_renderPass.m_sceneUBOSlot, uboDataBuffer, sizeof(SceneUBO));
  m_mainPool->SubmitUpdates();

   m_spherePool->BeginUpdates();
   m_spherePool->UpdateUniformData(m_renderPass.m_sphereId, m_renderPass.m_sceneUBOSlot, sphereDataBuffer,
                                   sizeof(SceneUBO));
   m_spherePool->SubmitUpdates();

   m_planePool->BeginUpdates();
   m_planePool->UpdateUniformData(m_renderPass.m_planeId, m_renderPass.m_sceneUBOSlot, planeDataBuffer,
     sizeof(SceneUBO));
   m_planePool->SubmitUpdates();

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
