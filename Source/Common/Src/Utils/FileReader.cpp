#include "Utils/FileReader.h"

#include <fstream>
#include <iterator>

namespace Azura {

Containers::Vector<U8> FileReader::GetFileContents(const String& filePath, Memory::Allocator& allocator) {
  std::ifstream fileStream(filePath, std::ios::binary);
  Containers::Vector<U8> buffer(allocator);

  fileStream.seekg(0, std::ios::end);
  buffer.Resize(U32(fileStream.tellg()));
  fileStream.seekg(0, std::ios::beg);

  buffer.Assign(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());

  fileStream.close();

  return buffer;
}

SizeType FileReader::GetFileSize(const String& filePath) {
  std::ifstream fileStream(filePath);

  fileStream.seekg(0, std::ios::end);
  const auto size = SizeType(fileStream.tellg());
  fileStream.close();

  return size;
}
}  // namespace Azura
