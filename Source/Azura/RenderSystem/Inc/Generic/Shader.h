#pragma once
#include "GenericTypes.h"

namespace Azura {

class Shader {
public:
  Shader(U32 id, const String& filePath, const String& extension);
  void SetStage(ShaderStage stage);
  void SetEntryPoint(const String& entryPoint);

  U32 GetId() const;

protected:
  const String& GetFilePath() const;
  const String& GetShaderEntryPoint() const;
  ShaderStage GetShaderStage() const;

private:
  U32 m_id;
  const String m_filePath;
  ShaderStage m_stage;
  String m_entryPoint;
};
} // namespace Azura
