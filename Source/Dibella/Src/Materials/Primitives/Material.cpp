#include "Material.h"
#include "../../Interfaces/ImageHandler.h"

namespace Dibella {
  Color3f Material::GetImageColor(const Point2f& uv, const ImageHandler* handle) {
    if (handle == nullptr) {
      return Color3f(1.0f);
    }

    return handle->GetPixel(UVToPixelSpace(uv, handle->GetWidth(), handle->GetHeight()));
  }
}
