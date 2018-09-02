#pragma once
#include "GenericTypes.h"

namespace Azura {

class Shader {
 public:
  Shader(const String& filePath, const String& extension);
  void SetStage(ShaderStage stage);
  void SetEntryPoint(const String& entryPoint);

 protected:
  const String& GetFilePath() const;
  const String& GetShaderEntryPoint() const;
  ShaderStage GetShaderStage() const;

 private:
  const String m_filePath;
  ShaderStage m_stage;
  String m_entryPoint;
};
}  // namespace Azura
