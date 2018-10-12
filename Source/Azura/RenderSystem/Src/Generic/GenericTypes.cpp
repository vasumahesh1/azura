#include "Generic/GenericTypes.h"

namespace Azura {

ShaderRequirements::ShaderRequirements(U32 numShaders, Memory::Allocator& allocator)
  : m_shaders(numShaders, allocator) {

}

U32 ShaderRequirements::AddShader(const ShaderCreateInfo& info) {
  m_shaders.PushBack(info);
  return m_shaders.GetSize() - 1;
}

RenderPassRequirements::RenderPassRequirements(U32 numRenderTargets, U32 numPasses, Memory::Allocator& alloc)
  : m_targets(numRenderTargets, alloc),
    m_passSequence(numPasses, alloc) {
}

U32 RenderPassRequirements::AddTarget(const RenderTargetCreateInfo& info) {
  m_targets.PushBack(info);
  return m_targets.GetSize() - 1;
}

U32 RenderPassRequirements::AddPass(const PipelinePassCreateInfo& info) {
  m_passSequence.PushBack(info);
  return m_passSequence.GetSize() - 1;
}

DescriptorRequirements::DescriptorRequirements(U32 numDescriptors, Memory::Allocator& alloc)
  : m_descriptorSlots(numDescriptors, alloc) {
}

U32 DescriptorRequirements::AddDescriptor(const DescriptorSlotCreateInfo& info) {
  m_descriptorSlots.PushBack(info);
  return m_descriptorSlots.GetSize() - 1;
}

} // namespace Azura
