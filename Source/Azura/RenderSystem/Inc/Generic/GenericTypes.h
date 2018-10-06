#pragma once

#include "Containers/Vector.h"
#include "Types.h"

#include <boost/detail/bitmask.hpp>

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

enum class ShaderStage : U32
{
  All = 0x00001111,
  Vertex = 0x00000001,
  Pixel = 0x00000010,
  Compute = 0x00000100,
  Geometry = 0x00001000
};

BOOST_BITMASK(ShaderStage);

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

enum class ImageType
{
  Image1D,
  Image2D,
  Image3D
};

enum class ImageViewType
{
  ImageView1D,
  ImageView1DArray,
  ImageView2D,
  ImageView2DArray,
  ImageView3D,
  ImageViewCubeMap,
  ImageViewCubeMapArray,
};

enum class SamplerType
{
  Sampler1D,
  Sampler2D,
  Sampler3D
};

struct Bounds3D {
  U32 m_width{0};
  U32 m_height{0};
  U32 m_depth{0};

  Bounds3D(U32 width, U32 height, U32 depth)
    : m_width(width),
    m_height(height),
    m_depth(depth) {

  }
};

struct Bounds2D {
  U32 m_width{0};
  U32 m_height{0};

  Bounds2D(U32 width, U32 height)
    : m_width(width),
    m_height(height) {

  }
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

struct LayerSubresource
{
  U32 m_layerCount{1};
  U32 m_baseLayer{0};
};

struct TextureSubresource
{
  LayerSubresource m_layerInfo{};
  U32 m_mipLevel{0};
};

}  // namespace Azura
