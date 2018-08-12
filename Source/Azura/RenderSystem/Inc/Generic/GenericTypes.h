#pragma once

#include "Types.h"

namespace Azura {
enum class RS {
  Success = 0,
  UnknownError,
};

enum class ColorSpace {
  SRGB,
  HDR10
};

enum class PresentModes {
  Immediate,
  Mailbox,
  FIFO,
  FIFORelaxed,
  SharedDemandRefesh,
  SharedContinuous
};

enum class ShaderStage {
  All,
  Vertex,
  Pixel,
  Compute,
  Geometry
};

enum class PrimitiveTopology {
  PointList,
  LineList,
  LineStrip,
  TriangleList,
  TriangleStrip,
  TriangleFan,
  LineListWithAdjacency,
  LineStripWithAdjacency,
  TriangleListWithAdjacency,
  TriangleStripWithAdjacency,
  PatchList
};

enum class CullMode {
  None,
  FrontBit,
  BackBit,
  FrontAndBack
};

enum class FrontFace {
  CounterClockwise,
  Clockwise
};

struct ShaderStageInfo {
  ShaderStage m_stage;
  String m_entryPoint;
  String m_filePath;
  bool m_isCompiled;
};

struct ViewportDimensions {
  float x;
  float y;
  float width;
  float height;
  float minDepth;
  float maxDepth;
};

} // namespace Azura
