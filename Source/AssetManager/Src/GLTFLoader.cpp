#include "AssetManager/GLTFLoader.h"

#include <cassert>
#include <sstream>
#include <boost/filesystem.hpp>

#include <GLTFSDK/IStreamReader.h>
#include <GLTFSDK/GLTFResourceReader.h>
#include <GLTFSDK/Deserialize.h>

#include "Log/Log.h"
#include "Utils/Macros.h"
#include "Memory/Allocator.h"


using namespace Microsoft::glTF;          // NOLINT
namespace filesystem = boost::filesystem; // NOLINT

namespace Azura {

namespace {
// The glTF SDK is decoupled from all file I/O by the IStreamReader (and IStreamWriter)
// interface(s) and the C++ stream-based I/O library. This allows the glTF SDK to be used in
// sandboxed environments, such as WebAssembly modules and UWP apps, where any file I/O code
// must be platform or use-case specific.
class StreamReader : public IStreamReader {
public:
  explicit StreamReader(filesystem::path pathBase)
    : m_pathBase(std::move(pathBase)) {
    assert(m_pathBase.has_root_path());
  }

  // Resolves the relative URIs of any external resources declared in the glTF manifest
  std::shared_ptr<std::istream> GetInputStream(const std::string& filename) const override {
    // In order to construct a valid stream:
    // 1. The filename argument will be encoded as UTF-8 so use filesystem::u8path to
    //    correctly construct a path instance.
    // 2. Generate an absolute path by concatenating m_pathBase with the specified filename
    //    path. The filesystem::operator/ uses the platform's preferred directory separator
    //    if appropriate.
    // 3. Always open the file stream in binary mode. The glTF SDK will handle any text
    //    encoding issues for us.
    auto streamPath = m_pathBase / filename;
    auto stream     = std::make_shared<std::ifstream>(streamPath.c_str(), std::ios_base::binary);

    // Check if the stream has no errors and is ready for I/O operations
    if (!stream || !(*stream)) {
      throw std::runtime_error("Unable to create a valid input stream for uri: " + filename);
    }

    return stream;
  }

private:
  filesystem::path m_pathBase;
};

} // namespace

GLTFMeshInterface::GLTFMeshInterface(const String& manifest,
                                     std::unique_ptr<const Microsoft::glTF::GLTFResourceReader> resourceReader)
  : m_resourceReader(std::move(resourceReader)),
    m_document(Deserialize(manifest)) {
}

std::vector<float> GLTFMeshInterface::GetPositionBuffer(U32 meshId,
                                                        U32 meshPrimitive,
                                                        U32& bufferSize,
                                                        U32& count) const {

  const auto& primitive = GetMeshPrimitive(GetMesh(meshId), meshPrimitive);

  const Accessor& accessor = m_document.accessors.Get(primitive.GetAttributeAccessorId("POSITION"));

  const auto data = m_resourceReader->ReadBinaryData<float>(m_document, accessor);

  count      = U32(accessor.count);
  bufferSize = U32(data.size() * sizeof(float));

  return data;
}

std::vector<float> GLTFMeshInterface::
GetNormalBuffer(U32 meshId, U32 meshPrimitive, U32& bufferSize, U32& count) const {
  const auto& primitive = GetMeshPrimitive(GetMesh(meshId), meshPrimitive);

  const Accessor& accessor = m_document.accessors.Get(primitive.GetAttributeAccessorId("NORMAL"));

  const auto data = m_resourceReader->ReadBinaryData<float>(m_document, accessor);

  count      = U32(accessor.count);
  bufferSize = U32(data.size() * sizeof(float));

  return data;
}

std::vector<float> GLTFMeshInterface::GetUVBuffer(U32 meshId, U32 meshPrimitive, U32& bufferSize, U32& count) const {
  const auto& primitive = GetMeshPrimitive(GetMesh(meshId), meshPrimitive);

  const Accessor& accessor = m_document.accessors.Get(primitive.GetAttributeAccessorId("TEXCOORD_0"));

  const auto data = m_resourceReader->ReadBinaryData<float>(m_document, accessor);

  count      = U32(accessor.count);
  bufferSize = U32(data.size() * sizeof(float));

  return data;
}

std::vector<U32> GLTFMeshInterface::GetIndexBuffer(U32 meshId, U32 meshPrimitive, U32& bufferSize, U32& count) const {

  const auto& primitive = GetMeshPrimitive(GetMesh(meshId), meshPrimitive);

  const Accessor& accessor = m_document.accessors.Get(primitive.indicesAccessorId);

  const auto data = m_resourceReader->ReadBinaryData<U32>(m_document, accessor);

  count      = U32(accessor.count);
  bufferSize = U32(data.size() * sizeof(U32));

  return data;
}

const Microsoft::glTF::Mesh& GLTFMeshInterface::GetMesh(U32 idx) const {
  return m_document.meshes[idx];
}

const Microsoft::glTF::MeshPrimitive& GLTFMeshInterface::GetMeshPrimitive(const Microsoft::glTF::Mesh& mesh, U32 idx) {
  return mesh.primitives[idx];
}

GLTFMeshInterface GLTFLoader::LoadFromJSON(const String& assetName, AssetLocation location, const Log& log) {
  UNUSED(location);
  UNUSED(log); // Release Mode

  filesystem::path filePath = filesystem::current_path() / filesystem::
                              path("Meshes/" + assetName + "/" + assetName + ".gltf");

  LOG_DBG(log, LOG_LEVEL, "Loading Mesh: %s", filePath.c_str());

  auto streamReader = std::make_unique<StreamReader>(filePath.parent_path());

  const auto gltfStream = streamReader->GetInputStream(assetName + ".gltf");
  auto resourceReader   = std::make_unique<GLTFResourceReader>(std::move(streamReader));

  std::stringstream manifestStream;

  // Read the contents of the glTF file into a string using a std::stringstream
  manifestStream << gltfStream->rdbuf();
  const std::string manifest = manifestStream.str();

  return GLTFMeshInterface(manifest, std::move(resourceReader));
}

} // namespace Azura
