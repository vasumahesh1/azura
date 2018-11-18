#pragma once

#include "Containers/Vector.h"
#include "Generic/Constants.h"
#include "Types.h"
#include "Core/RawStorageFormat.h"
#include "Utils/Hash.h"

#include <boost/container/small_vector.hpp>
#include <boost/detail/bitmask.hpp>

namespace Azura {
template <typename T, SizeType N>
using SmallVector = boost::container::small_vector<T, N>;

const U32 PRESENT_TARGET = 100;

enum class RawStorageFormat;

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
  SharedDemandRefresh,
  SharedContinuous
};

enum class PipelineType
{
  Graphics,
  Compute
};

enum class RenderPassType
{
  Graphics,
  Compute
};

enum class ShaderStage : U32 {
  All = 0x00001111,
  Vertex = 0x00000001,
  Pixel = 0x00000010,
  Compute = 0x00000100,
  Geometry = 0x00001000
};

BOOST_BITMASK(ShaderStage);

enum class BufferUsage {
  Vertex,
  Index,
};

enum class BufferUsageRate {
  PerVertex,
  PerInstance
};

enum class BlendFactor {
  Zero,
  One,
  SrcColor,
  OneMinusSrcColor,
  DstColor,
  OneMinusDstColor,
  SrcAlpha,
  OneMinusSrcAlpha,
  DstAlpha,
  OneMinusDstAlpha,
  ConstantColor,
  OneMinusConstantColor,
  ConstantAlpha,
  OneMinusConstantAlpha,
  SrcAlphaSaturate,
  Src1Color,
  OneMinusSrc1Color,
  Src1Alpha,
  OneMinusSrc1Alpha
};

enum class BlendOp {
  Add,
  Subtract,
  ReverseSubtract,
  Min,
  Max
};

struct BlendingEq {
  BlendFactor m_srcFactor{};
  BlendFactor m_dstFactor{};
  BlendOp m_op{};
};

struct BlendState {
  bool m_enable{false};
  BlendingEq m_color{};
  BlendingEq m_alpha{};
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

enum class DrawType {
  InstancedIndexed,
  InstancedIndexedIndirect
};

enum class ImageType {
  Image1D,
  Image2D,
  Image3D
};

enum class ImageViewType {
  ImageView1D,
  ImageView1DArray,
  ImageView2D,
  ImageView2DArray,
  ImageView3D,
  ImageViewCubeMap,
  ImageViewCubeMapArray,
};

enum class SamplerType {
  Sampler1D,
  Sampler2D,
  Sampler3D
};

enum class DescriptorType {
  UniformBuffer,
  Sampler,
  SampledImage,
  CombinedImageSampler,
  PushConstant,
  UnorderedView
};

enum class BufferSource {
  Normal,
  StructuredBuffer
};

const U32 MAX_DESCRIPTOR_TYPE_COUNT = 5;

struct Bounds3D {
  U32 m_width{1};
  U32 m_height{1};
  U32 m_depth{1};

  Bounds3D(U32 width, U32 height, U32 depth)
    : m_width(width),
      m_height(height),
      m_depth(depth) {

  }

  U32 GetSize() const {
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

using SlotID = U32;

struct SlotSemantic {
  const char* m_name;
  RawStorageFormat m_format;
  U32 m_id{0};
};

struct VertexSlot {
  BufferUsageRate m_rate;
  SmallVector<SlotSemantic, 10> m_stride;
  U32 m_strideSize{0};
  BufferSource m_bufferSource{BufferSource::Normal};
};

struct DescriptorSlotCreateInfo {
  DescriptorType m_type{DescriptorType::UniformBuffer};
  ShaderStage m_stages{ShaderStage::All};
};

struct DescriptorSlot {
  U32 m_key{0};
  DescriptorType m_type{DescriptorType::UniformBuffer};
  ShaderStage m_stages{ShaderStage::All};
  U32 m_setIdx{0};
  U32 m_bindIdx{0};
};

struct TextureDesc {
  Bounds3D m_bounds{1, 1, 1};
  U32 m_size{0};
  U32 m_mipLevels{1};
  U32 m_arrayLayers{1};
  ImageType m_type{ImageType::Image2D};
  RawStorageFormat m_format{RawStorageFormat::R8G8B8A8_UNORM};
};

enum class TextureAddressMode {
  Wrap,
  Mirror,
  Clamp,
  Border,
  MirrorOnce
};

enum class TextureFilter {
  MinMagMipPoint,
  MinMagPoint_MipLinear,
  MinPoint_MagLinear_MipPoint,
  MinPoint_MagMipLinear,
  MinLinear_MagMipPoint,
  MinLinear_MagPoint_MipLinear,
  MinMagLinear_MipPoint,
  MinMagMipLinear,
  Anisotropic,
};

struct SamplerDesc {
  TextureAddressMode m_addressModeU{TextureAddressMode::Wrap};
  TextureAddressMode m_addressModeV{TextureAddressMode::Wrap};
  TextureAddressMode m_addressModeW{TextureAddressMode::Wrap};

  TextureFilter m_filter{TextureFilter::MinMagMipLinear};
};

struct BufferInfo {
  U32 m_offset;
  U32 m_byteSize;
  U32 m_maxByteSize;
  U32 m_binding;
  U32 m_sourceBufferId;

  BufferInfo()
    : m_offset(0),
      m_byteSize(0),
      m_maxByteSize(0),
      m_binding(0),
      m_sourceBufferId(0) {
  }
};

enum class BufferUpdateType {
  UniformBuffer,
  Sampler,
  SampledImage,
  CombinedImageSampler,
  PushConstant,
  UnorderedView,
  Vertex,
  Instance
};

struct BufferUpdate
{
  BufferUpdateType m_type;

  U32 m_idx;
  U32 m_gpuOffset;
  U32 m_gpuByteSize;

  U32 m_updateOffset;
  U32 m_updateByteSize;
};

struct TextureBufferInfo final : public BufferInfo {
  TextureDesc m_desc;
  U32 m_set{0};
};

struct SamplerInfo {
  U32 m_set{0};
  U32 m_binding{0};
  SamplerDesc m_desc{};
};

struct UniformBufferInfo final : public BufferInfo {
  U32 m_set{0};
};

struct BufferTargetInfo final : public BufferInfo {
  U32 m_set{0};
};


struct LayerSubresource {
  U32 m_layerCount{1};
  U32 m_baseLayer{0};
};

struct TextureSubresource {
  LayerSubresource m_layerInfo{};
  U32 m_mipLevel{0};
};

struct TextureRequirements {
  U32 m_maxCount{0};
  U32 m_poolSize{0};
};

enum class AssetLocation {
  Shaders,
  Textures,
  Audio,
  Meshes
};

struct DeviceRequirements {
  bool m_discreteGPU{true};
  bool m_transferQueue{false};
  bool m_float64{false};
  bool m_int64{false};
};

struct ApplicationInfo {
  std::string m_name;
  Version m_version;
};

constexpr U32 DEFAULT_FRAMES_IN_FLIGHT = 2;
constexpr U32 MAX_RENDER_PASS_INPUTS   = 4;
constexpr U32 MAX_RENDER_PASS_OUTPUTS  = 4;
constexpr U32 MAX_RENDER_PASS_SETS     = 8;
constexpr U32 MAX_RENDER_PASS_SHADERS  = 4;

struct MemoryRequirements {
  U32 m_stagingBufferSize{0x4000000};
};

struct ApplicationRequirements {
  MemoryRequirements m_renderer;
};

struct SwapChainRequirements {
  RawStorageFormat m_format{};
  RawStorageFormat m_depthFormat{};
  Bounds2D m_extent{0u, 0u};
  ColorSpace m_colorSpace{};
  U32 m_framesInFlight{DEFAULT_FRAMES_IN_FLIGHT};
};

struct ShaderCreateInfo {
  ShaderStage m_stage;
  String m_shaderFileName;
  AssetLocation location;
};

struct RenderTargetCreateInfo {
  RawStorageFormat m_format{RawStorageFormat::UNKNOWN};
  int m_width{-1};
  int m_height{-1};
  int m_depth{1};
};

struct StructuredBufferCreateInfo {
  U32 m_size;
  U32 m_stride;
};

struct PipelinePassInput {
  U32 m_id;
  ShaderStage m_stages;
};

struct ClearData {
  float m_color[4]{0.0f, 0.0f, 0.0f, 1.0f};
  float m_depth{1.0f};
  U8 m_stencil{0};
};

struct PipelinePassCreateInfo {
  using Shaders = SmallVector<U32, MAX_RENDER_PASS_SHADERS>;
  using OutputTargets = SmallVector<U32, MAX_RENDER_PASS_OUTPUTS>;
  using OutputBuffers = SmallVector<U32, MAX_RENDER_PASS_OUTPUTS>;
  using InputTargets = SmallVector<PipelinePassInput, MAX_RENDER_PASS_INPUTS>;
  using InputBuffers = SmallVector<PipelinePassInput, MAX_RENDER_PASS_INPUTS>;
  using DescriptorSets = SmallVector<U32, MAX_RENDER_PASS_SETS>;

  Shaders m_shaders{};

  InputTargets m_inputTargets{};
  InputBuffers m_inputBuffers{};
  OutputTargets m_outputTargets{};
  OutputBuffers m_outputBuffers{};
  DescriptorSets m_descriptorSets{};

  ClearData m_clearData{};
  BlendState m_blendState{};
  RenderPassType m_type{RenderPassType::Graphics};
};

struct DescriptorRequirements {
  DescriptorRequirements(U32 numDescriptors, U32 numSets, Memory::Allocator& alloc);
  U32 AddDescriptor(const DescriptorSlotCreateInfo& info);
  U32 AddSet(const std::initializer_list<U32>& sets);

  Containers::Vector<DescriptorSlotCreateInfo> m_descriptorSlots;
  Containers::Vector<Containers::Vector<U32>> m_descriptorSets;

private:
  std::reference_wrapper<Memory::Allocator> m_allocator;
};

struct ShaderRequirements {
  ShaderRequirements(U32 numShaders, Memory::Allocator& allocator);
  U32 AddShader(const ShaderCreateInfo& info);

  Containers::Vector<ShaderCreateInfo> m_shaders;
};

struct RenderPassRequirements {
  RenderPassRequirements(U32 numRenderTargets, U32 numPasses, U32 numBuffers, Memory::Allocator& alloc);
  U32 AddTarget(const RenderTargetCreateInfo& info);
  U32 AddBuffer(const StructuredBufferCreateInfo& info);
  U32 AddPass(const PipelinePassCreateInfo& info);

  Containers::Vector<StructuredBufferCreateInfo> m_buffers;
  Containers::Vector<RenderTargetCreateInfo> m_targets;
  Containers::Vector<PipelinePassCreateInfo> m_passSequence;

  U32 m_maxPools{1};
};

struct DescriptorCount {
  U32 m_numUniformSlots{0};
  U32 m_numSamplerSlots{0};
  U32 m_numCombinedSamplerSlots{0};
  U32 m_numSampledImageSlots{0};
  U32 m_numPushConstantsSlots{0};
  U32 m_numUnorderedViewSlots{0};
};

struct ThreadGroupDimensions
{
  U32 m_x{1};
  U32 m_y{1};
  U32 m_z{1};
};

struct DescriptorTableEntry {
  int m_count{1};
  int m_cumulativeCount{0};
  DescriptorType m_type;
};

} // namespace Azura
