#include "D3D12/D3D12ScopedPipeline.h"
#include "D3D12/D3D12Macros.h"
#include "D3D12/D3D12TypeMapping.h"


namespace Azura {
namespace D3D12 {

D3D12ScopedPipeline::D3D12ScopedPipeline(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
                                         D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc,
                                         const Log& log) {

  VERIFY_D3D_OP(log, device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipeline)),
    "Failed to create pipeline");
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> D3D12ScopedPipeline::Real() const {
  return m_pipeline;
}

D3D12PipelineFactory::D3D12PipelineFactory(Memory::Allocator& allocator, Log logger)
  : log_D3D12RenderSystem(std::move(logger)),
    m_inputElementDescs(10, allocator),
    m_vertexShaderModule(),
    m_pixelShaderModule() {
}

D3D12PipelineFactory& D3D12PipelineFactory::BulkAddAttributeDescription(const VertexSlot& vertexSlot, U32 binding) {

  auto bindingInfo = m_bindingMap[binding];

  for (const auto& semanticStride : vertexSlot.m_stride) {
    const auto format = ToDXGI_FORMAT(semanticStride.m_format);
    VERIFY_OPT(log_D3D12RenderSystem, format, "Unknown Format");

    const auto usageRate = ToD3D12_INPUT_CLASSIFICATION(vertexSlot.m_rate);
    VERIFY_OPT(log_D3D12RenderSystem, usageRate, "Unknown Buffer Usage Rate");

    D3D12_INPUT_ELEMENT_DESC attrDesc;
    attrDesc.Format = format.value();
    attrDesc.SemanticName = semanticStride.m_name;
    attrDesc.SemanticIndex = semanticStride.m_id;
    attrDesc.InputSlot = binding;
    attrDesc.AlignedByteOffset = bindingInfo.m_offset;
    attrDesc.InputSlotClass = usageRate.value();
    attrDesc.InstanceDataStepRate = 0;

    bindingInfo.m_offset += GetFormatSize(semanticStride.m_format);

    m_inputElementDescs.PushBack(attrDesc);
  }

  m_bindingMap[binding] = bindingInfo;

  return *this;
}

D3D12PipelineFactory& D3D12PipelineFactory::SetInputAssemblyStage(PrimitiveTopology topology) {
}

D3D12PipelineFactory& D3D12PipelineFactory::AddShaderStage(const D3D12ScopedShader& shader) {
}

void D3D12PipelineFactory::Submit(ID3D12RootSignature* rootSignature) const {
}
} // namespace D3D12
} // namespace Azura
