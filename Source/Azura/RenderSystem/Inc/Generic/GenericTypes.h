#pragma once

#include "Containers/Vector.h"
#include "Types.h"
#include "Core/RawStorageFormat.h"

#include <boost/detail/bitmask.hpp>
#include "Utils/Hash.h"

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

enum class DescriptorType
{
  UniformBuffer,
  Sampler,
  SampledImage,
  CombinedImageSampler,
  PushConstant
};

const U32 MAX_DESCRIPTOR_TYPE_COUNT = 5;

enum class DescriptorBinding
{
  Default,
  Same
};

struct Bounds3D {
  U32 m_width{1};
  U32 m_height{1};
  U32 m_depth{1};

  Bounds3D(U32 width, U32 height, U32 depth)
    : m_width(width),
    m_height(height),
    m_depth(depth) {

  }

  U32 GetSize() const
  {
    return m_width * m_height * m_depth;
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

using SlotID = SizeType;

constexpr SlotID GenSlot(const char* str)
{
  return FNVHash(str, sizeof(str), DEFAULT_SEED, DEFAULT_PRIME);
}

struct VertexSlot {
  SizeType m_key;
  BufferUsageRate m_rate;
};

struct DescriptorSlotCreateInfo {
  SizeType m_key{0};
  DescriptorType m_type{DescriptorType::UniformBuffer};
  ShaderStage m_stages{ShaderStage::All};
  DescriptorBinding m_binding{ DescriptorBinding::Default };
};

struct DescriptorSlot
{
  SizeType m_key{0};
  DescriptorType m_type{DescriptorType::UniformBuffer};
  ShaderStage m_stages{ShaderStage::All};
  DescriptorBinding m_binding{ DescriptorBinding::Default };
  U32 m_setIdx{0};
  U32 m_bindIdx{0};
};

struct TextureDesc
{
  Bounds3D m_bounds{ 1, 1, 1 };
  U32 m_size{0};
  U32 m_mipLevels{1};
  U32 m_arrayLayers{1};
  ImageType m_type{ImageType::Image2D};
  RawStorageFormat m_format{ RawStorageFormat::R8G8B8A8_UNORM };
};

struct SamplerDesc
{
};

struct BufferInfo {
  U32 m_offset;
  U32 m_byteSize;
  U32 m_maxByteSize;
  U32 m_binding;

  BufferInfo() : m_offset(0), m_byteSize(0), m_maxByteSize(0), m_binding(0) {}
};

struct TextureBufferInfo final : public BufferInfo
{
  TextureDesc m_desc;
  U32 m_set{0};
};

struct SamplerInfo
{
  U32 m_set{0};
  U32 m_binding{0};
};

struct UniformBufferInfo final : public BufferInfo
{
  U32 m_set{0};
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

struct TextureRequirements
{
  U32 m_maxCount{ 0 };
  U32 m_poolSize{ 0 };
};

}  // namespace Azura
