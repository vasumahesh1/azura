#pragma once
#include "Types.h"

#include "Containers/Vector.h"

namespace Azura {
namespace Memory {
class Allocator;
}
} // namespace Azura

namespace Azura {

namespace FileReader {
Containers::Vector<U8> GetFileContents(const String& filePath, Memory::Allocator& allocator);

void GetFileContents(const String & filePath, Containers::Vector<U8>& buffer);

SizeType GetFileSize(const String& filePath);
} // namespace FileReader

} // namespace Azura
