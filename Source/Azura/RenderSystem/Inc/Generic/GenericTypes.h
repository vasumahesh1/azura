#pragma once

#include "Types.h"
#include "Containers/Vector.h"

namespace Azura {
enum class RawStorageFormat;

enum class RS {
  Success = 0,
  UnknownError,
};

enum class ColorSpace { SRGB, HDR10 };

enum class PresentModes { Immediate, Mailbox, FIFO, FIFORelaxed, SharedDemandRefresh, SharedContinuous };

enum class ShaderStage { All, Vertex, Pixel, Compute, Geometry };

enum class BufferUsage {
  Vertex,
  Index,
};

enum class BufferUsageRate { PerVertex, PerInstance };

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

enum class CullMode { None, FrontBit, BackBit, FrontAndBack };

enum class FrontFace { CounterClockwise, Clockwise };

enum class DrawType { InstancedIndexed, InstancedIndexedIndirect };

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

struct Slot {
  U32 m_binding;
  BufferUsageRate m_rate;
};

struct BufferInfo {
  U32 m_offset;
  U32 m_byteSize;

  Containers::Vector<RawStorageFormat> m_strideInfo;
  Slot m_slot;

  explicit BufferInfo(Memory::Allocator& alloc)
    : m_offset(0),
      m_byteSize(0),
      m_strideInfo(alloc),
      m_slot() {
  }
};

} // namespace Azura
