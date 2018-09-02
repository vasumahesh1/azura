#pragma once

#include <stdexcept>

namespace Azura {

class NotImplementedException : public std::logic_error {
 public:
  virtual char const *what() const {
    return "Function or branch not yet implemented.";
  }
};

}  // namespace Azura