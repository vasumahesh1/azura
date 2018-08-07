#pragma once
#include "../Core/Types.h"

namespace Dibella {
  class ImageHandler {
  public:
    virtual ~ImageHandler() = default;

    ImageHandler(const ImageHandler& other) = default;
    ImageHandler(ImageHandler&& other) noexcept = default;
    ImageHandler& operator=(const ImageHandler& other) = default;
    ImageHandler& operator=(ImageHandler&& other) noexcept = default;

    virtual SizeType GetWidth() const = 0;
    virtual SizeType GetHeight() const = 0;
    virtual Color3f GetPixel(Point2i pixelCoords) const = 0;
  };
}
