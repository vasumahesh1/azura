#pragma once
#include "Generic/Shader.h"
#include "Log/Log.h"
#include "D3D12/D3D12Core.h"

#include <wrl/client.h>
#include "d3dx12.h"


namespace Azura {
namespace D3D12 {

class D3D12ScopedShader final : public Shader {
public:
  D3D12ScopedShader(const String& filePath, Memory::Allocator& shaderCodeAllocator, Log log);

  CD3DX12_SHADER_BYTECODE GetByteCode() const;

private:
  Log log_D3D12RenderSystem;

  Containers::Vector<U8> m_byteCode;
};

} // namespace D3D12
} // namespace Azura
