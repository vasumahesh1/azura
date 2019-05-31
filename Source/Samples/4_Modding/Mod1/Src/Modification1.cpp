#include "Mod1/Modification1.h"

namespace Mod1 {

Azura::String Modification1::GetName() {
  return "Modification1";
}

int Modification1::DoMath(int a, int b) const {
  return a + b;
}
} // namespace Mod1