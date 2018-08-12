#include "Utils/FileReader.h"

#include <iterator>
#include <fstream>

namespace Azura {

std::vector<U8> FileReader::GetFileContents(const String& filePath) {
  std::ifstream fileStream(filePath);
  std::vector<U8> buffer;

  fileStream.seekg(0, std::ios::end);
  buffer.reserve(fileStream.tellg());
  fileStream.seekg(0, std::ios::beg);

  buffer.assign(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());

  fileStream.close();

  return buffer;
}

SizeType FileReader::GetFileSize(const String& filePath) {
  std::ifstream fileStream(filePath);
  std::vector<U8> buffer;

  fileStream.seekg(0, std::ios::end);
  const SizeType size = fileStream.tellg();
  fileStream.close();

  return size;
}
} // namespace Azura
