#include "D3D12/D3D12ScopedPipeline.h"
#include "D3D12/D3D12Macros.h"
#include "D3D12/D3D12TypeMapping.h"
#include "D3D12/D3D12ScopedRenderPass.h"
#include "D3D12/D3D12ScopedComputePass.h"


namespace Azura {
namespace D3D12 {

D3D12ScopedPipeline::D3D12ScopedPipeline(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
                                         D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc,
                                         const Log& log) : m_type(PipelineType::Graphics) {

  VERIFY_D3D_OP(log, device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipeline)),
    "Failed to create graphics pipeline");
}

D3D12ScopedPipeline::D3D12ScopedPipeline(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
  D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc,
  const Log& log) : m_type(PipelineType::Compute) {

  VERIFY_D3D_OP(log, device->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&m_pipeline)),
    "Failed to create compute pipeline");
}

ID3D12PipelineState* D3D12ScopedPipeline::GetState() const {
  return m_pipeline.Get();
}

PipelineType D3D12ScopedPipeline::GetType() const {
  return m_type;
}

D3D12PipelineFactory::D3D12PipelineFactory(Memory::Allocator& allocator, Log logger)
  : log_D3D12RenderSystem(std::move(logger)),
    m_type(PipelineType::Graphics),
    m_inputElementDescs(10, allocator) {
}

D3D12PipelineFactory& D3D12PipelineFactory::SetPipelineType(PipelineType type) {
  m_type = type;

  return *this;
}

D3D12PipelineFactory& D3D12PipelineFactory::BulkAddAttributeDescription(const VertexSlot& vertexSlot, U32 binding) {

  auto bindingInfo = m_bindingMap[binding];

  for (const auto& semanticStride : vertexSlot.m_stride) {
    const auto format = ToDXGI_FORMAT(semanticStride.m_format);
    VERIFY_OPT(log_D3D12RenderSystem, format, "Unknown Format");

    const auto usageRate = ToD3D12_INPUT_CLASSIFICATION(vertexSlot.m_rate);
    VERIFY_OPT(log_D3D12RenderSystem, usageRate, "Unknown Buffer Usage Rate");

    LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "Binding Vertex Attribute: Binding: %d  Semantic: %s  Format: %s", binding, semanticStride.m_name, ToString(semanticStride.m_format).c_str());

    D3D12_INPUT_ELEMENT_DESC attrDesc;
    attrDesc.Format = format.value();
    attrDesc.SemanticName = semanticStride.m_name;
    attrDesc.SemanticIndex = semanticStride.m_id;
    attrDesc.InputSlot = binding;
    attrDesc.AlignedByteOffset = bindingInfo.m_offset;
    attrDesc.InputSlotClass = usageRate.value();
    attrDesc.InstanceDataStepRate = vertexSlot.m_rate == BufferUsageRate::PerInstance ? 1 : 0;

    bindingInfo.m_offset += GetFormatSize(semanticStride.m_format);

    m_inputElementDescs.PushBack(attrDesc);
  }

  m_bindingMap[binding] = bindingInfo;

  return *this;
}

D3D12PipelineFactory& D3D12PipelineFactory::SetRasterizerStage(CullMode cullMode, FrontFace faceOrder) {
  const auto d3dCullMode = ToD3D12_CULL_MODE(cullMode);
  VERIFY_OPT(log_D3D12RenderSystem, d3dCullMode, "Unknown D3D12 Cull Mode");

  m_rasterizerDesc.CullMode = d3dCullMode.value();
  m_rasterizerDesc.FrontCounterClockwise = static_cast<BOOL>(faceOrder == FrontFace::CounterClockwise);

  return *this;
}

D3D12PipelineFactory& D3D12PipelineFactory::AddShaderStage(const D3D12ScopedShader& shader) {
  switch(shader.GetShaderStage())
  {
    case ShaderStage::Vertex:
      m_vertexShaderModule = shader.GetByteCode();
      break;

    case ShaderStage::Pixel:
      m_pixelShaderModule = shader.GetByteCode();
      break;

    case ShaderStage::Compute:
      m_computeShaderModule = shader.GetByteCode();
      break;

    case ShaderStage::Geometry:
    case ShaderStage::All:
      LOG_ERR(log_D3D12RenderSystem, LOG_LEVEL, "Unsupported Shader Stage for Pipeline Factory");
      break;

    default:
      LOG_ERR(log_D3D12RenderSystem, LOG_LEVEL, "Unknown Shader Stage for Pipeline Factory");
    break;
  }

  return *this;
}

void D3D12PipelineFactory::Submit(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Containers::Vector<std::reference_wrapper<D3D12ScopedRenderPass>>& renderPasses, Containers::Vector<D3D12ScopedPipeline>& resultPipelines) const {
  assert(m_type == PipelineType::Graphics);

  for (const auto& renderPass : renderPasses)
  {
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { m_inputElementDescs.Data(), m_inputElementDescs.GetSize() };
    psoDesc.pRootSignature = renderPass.get().GetRootSignature();


    for (const auto& shader : renderPass.get().GetShaders())
    {
      switch (shader.get().GetShaderStage())
      {
      case ShaderStage::All: break;

      case ShaderStage::Vertex:
        psoDesc.VS = shader.get().GetByteCode();
        break;

      case ShaderStage::Pixel:
        psoDesc.PS = shader.get().GetByteCode();
        break;

      case ShaderStage::Compute:
        break;

      case ShaderStage::Geometry:
        psoDesc.GS = shader.get().GetByteCode();
        break;

      default: break;
      }
    }

    if (m_vertexShaderModule.has_value())
    {
      psoDesc.VS = m_vertexShaderModule.value();
    }

    if (m_pixelShaderModule.has_value())
    {
      psoDesc.PS = m_pixelShaderModule.value();
    }

    psoDesc.RasterizerState = m_rasterizerDesc;

    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.SampleDesc.Count = 1;

    // Update Output Targets
    renderPass.get().UpdatePipelineInfo(psoDesc);

    resultPipelines.PushBack(D3D12ScopedPipeline(device, psoDesc, log_D3D12RenderSystem));
  }

}

void D3D12PipelineFactory::Submit(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
  const Containers::Vector<std::reference_wrapper<D3D12ScopedComputePass>>& computePasses,
  Containers::Vector<D3D12ScopedPipeline>& resultPipelines) const {
  
  assert(m_type == PipelineType::Compute);

  for (const auto& computePass : computePasses)
  {
    D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = computePass.get().GetRootSignature();


    for (const auto& shader : computePass.get().GetShaders())
    {
      switch (shader.get().GetShaderStage())
      {
      case ShaderStage::Compute:
        psoDesc.CS = shader.get().GetByteCode();
        break;

      case ShaderStage::All:
      case ShaderStage::Vertex:
      case ShaderStage::Pixel:
      case ShaderStage::Geometry:
      default:
        break;
      }
    }

    if (m_computeShaderModule.has_value())
    {
      psoDesc.CS = m_computeShaderModule.value();
    }

    resultPipelines.PushBack(D3D12ScopedPipeline(device, psoDesc, log_D3D12RenderSystem));
  }
}
} // namespace D3D12
} // namespace Azura
