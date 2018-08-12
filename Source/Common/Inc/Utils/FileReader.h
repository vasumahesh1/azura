#pragma once
#include "Types.h"

#include <vector>

namespace Azura {

namespace FileReader {
std::vector<U8> GetFileContents(const String& filePath);

SizeType GetFileSize(const String& filePath);
} // namespace FileReader

} // namespace Azura
