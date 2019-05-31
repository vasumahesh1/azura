#pragma once
#include "Types.h"

#define REGISTER_MOD(ModName) \
static ModName* Create() { \
    return new ModName(); \
}

namespace Azura {
class Log;
}

namespace Azura {
namespace Modding {

class Mod {
public:
  Mod() = default;
  virtual ~Mod() = default;

  Mod(const Mod& other) = delete;
  Mod(Mod&& other) noexcept = delete;
  Mod& operator=(const Mod& other) = delete;
  Mod& operator=(Mod&& other) noexcept = delete;

  virtual String GetName() = 0;

  virtual void OnLoad(const Log& log);
  virtual void OnUnload(const Log& log);

private:

};

} // namespace Modding
} // namespace Azura
