#include "D3D12/D3D12ScopedShader.h"
#include "Log/Log.h"

#include "Memory/MonotonicAllocator.h"
#include "Memory/MemoryFactory.h"
#include "Utils/FileReader.h"

namespace Azura {
namespace D3D12 {
namespace {
const String DXGB_EXTENSION = "dxbc";
} // namespace

D3D12ScopedShader::D3D12ScopedShader(const String& filePath, Memory::Allocator& shaderCodeAllocator, Log log)
  : Shader(filePath, DXGB_EXTENSION),
    log_D3D12RenderSystem(std::move(log)),
    m_byteCode(shaderCodeAllocator) {

  LOG_DEBUG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12ScopedShader: Loading Shader: %s", GetFilePath().c_str());

  FileReader::GetFileContents(GetFilePath(), m_byteCode);

  LOG_DEBUG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12ScopedShader: File Loaded: %s", GetFilePath().c_str());
  LOG_DEBUG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12ScopedShader: Successfully Created Shader: %s", GetFilePath().c_str(
  ));
}

CD3DX12_SHADER_BYTECODE D3D12ScopedShader::GetByteCode() const {
  return CD3DX12_SHADER_BYTECODE(m_byteCode.Data(), m_byteCode.GetSize());
}

} // namespace D3D12
} // namespace Azura
