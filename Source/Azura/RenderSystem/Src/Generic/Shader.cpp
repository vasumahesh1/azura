#include "Generic/Shader.h"

namespace Azura {

Shader::Shader(const String& filePath, const String& extension)
  : m_filePath(filePath + "." + extension),
    m_stage(),
    m_entryPoint("main") {
}

void Shader::SetStage(const ShaderStage stage) {
  m_stage = stage;
}

void Shader::SetEntryPoint(const String& entryPoint) {
  m_entryPoint = entryPoint;
}

const String& Shader::GetFilePath() const {
  return m_filePath;
}

const String& Shader::GetShaderEntryPoint() const {
  return m_entryPoint;
}

ShaderStage Shader::GetShaderStage() const {
  return m_stage;
}

} // namespace Azura
