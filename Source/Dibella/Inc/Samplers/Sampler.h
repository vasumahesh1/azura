#pragma once

#include "../Core/Types.h"
#include <cinttypes>

namespace Dibella {

  class Sampler {
  public:
    // Sampler Interface
    virtual ~Sampler();
    explicit Sampler(U64 samplesPerPixel);

    virtual Float Get1D() = 0;
    virtual Point2f Get2D() = 0;

    virtual std::unique_ptr<Sampler> Clone(int seed) = 0;
    virtual bool SetSampleNumber(U64 sampleNum);
    std::string StateString() const {
      return StringPrintf("(%d,%d), sample %" PRId64, currentPixel.x,
        currentPixel.y, currentPixelSampleIndex);
    }

    U64 CurrentSampleNumber() const { return currentPixelSampleIndex; }

    // Sampler Public Data
    const U64 samplesPerPixel;

  protected:
    // Sampler Protected Data
    Point2i currentPixel;
    U64 currentPixelSampleIndex;
    std::vector<int> samples1DArraySizes, samples2DArraySizes;
    std::vector<std::vector<Float>> sampleArray1D;
    std::vector<std::vector<Point2f>> sampleArray2D;

  private:
    // Sampler Private Data
    size_t array1DOffset, array2DOffset;
  };
}
