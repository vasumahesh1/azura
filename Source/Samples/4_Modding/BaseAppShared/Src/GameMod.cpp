#include "BaseAppShared/GameMod.h"
#include "Log/Log.h"
#include "Utils/Macros.h"

using namespace Azura; // NOLINT

namespace MyApp {

void GameMod::OnLoad(const Log& log) {
  UNUSED(log); // For Release
  LOG_DBG(log, LOG_LEVEL, "Loading Game Mod");
}

void GameMod::OnUnload(const Log& log) {
  UNUSED(log); // For Release
  LOG_DBG(log, LOG_LEVEL, "Unloading Game Mod");
}
} // namespace MyApp