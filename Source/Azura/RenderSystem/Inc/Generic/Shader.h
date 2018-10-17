#pragma once
#include "GenericTypes.h"

namespace Azura {

class Shader {
public:
  Shader(const String& filePath, const String& extension);
  void SetStage(ShaderStage stage);
  void SetEntryPoint(const String& entryPoint);

  ShaderStage GetShaderStage() const;

protected:
  const String& GetFilePath() const;
  const String& GetShaderEntryPoint() const;

private:
  const String m_filePath;
  ShaderStage m_stage;
  String m_entryPoint;
};
} // namespace Azura
