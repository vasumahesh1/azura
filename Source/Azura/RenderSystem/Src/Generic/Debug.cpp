#include "Generic/Debug.h"

namespace Azura {

const char* DescriptorTypeToString(DescriptorType type) {
  switch (type) {
    CASE_STR(DescriptorType::UniformBuffer);
    CASE_STR(DescriptorType::Sampler);
    CASE_STR(DescriptorType::SampledImage);
    CASE_STR(DescriptorType::CombinedImageSampler);
    CASE_STR(DescriptorType::PushConstant);
    CASE_STR(DescriptorType::UnorderedView);

    default:
      return "Unknown Descriptor Type";
  }
}

} // namespace Azura
