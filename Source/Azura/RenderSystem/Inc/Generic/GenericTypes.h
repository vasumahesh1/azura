#pragma once

#include "Containers/Vector.h"
#include "Types.h"

namespace Azura {
enum class RawStorageFormat;

enum class RS
{
  Success = 0,
  UnknownError,
};

enum class ColorSpace
{
  SRGB,
  HDR10
};

enum class PresentModes
{
  Immediate,
  Mailbox,
  FIFO,
  FIFORelaxed,
  SharedDemandRefresh,
  SharedContinuous
};

enum class ShaderStage
{
  All,
  Vertex,
  Pixel,
  Compute,
  Geometry
};

enum class BufferUsage
{
  Vertex,
  Index,
};

enum class BufferUsageRate
{
  PerVertex,
  PerInstance
};

enum class PrimitiveTopology
{
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

enum class CullMode
{
  None,
  FrontBit,
  BackBit,
  FrontAndBack
};

enum class FrontFace
{
  CounterClockwise,
  Clockwise
};

enum class DrawType
{
  InstancedIndexed,
  InstancedIndexedIndirect
};

struct ShaderStageInfo {
  ShaderStage m_stage;
  String m_entryPoint;
  String m_filePath;
  bool m_isCompiled;
};

struct ViewportDimensions {
  float m_x;
  float m_y;
  float m_width;
  float m_height;
  float m_minDepth;
  float m_maxDepth;
};

struct Slot {
  U32 m_binding;
  BufferUsageRate m_rate;
};

struct BufferInfo {
  U32 m_offset;
  U32 m_byteSize;
  U32 m_maxByteSize;
  Slot m_slot;

  BufferInfo() : m_offset(0), m_byteSize(0), m_maxByteSize(0), m_slot() {}
};

}  // namespace Azura
