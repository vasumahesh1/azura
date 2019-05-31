#include <iostream>

#include "Log/Log.h"
#include "ModFactory/ModManager.h"
#include "BaseAppShared/GameMod.h"

using GameMod = MyApp::GameMod;

int main() {
  auto log_AppMain = Azura::Log("BaseApp");

  try {
    LOG_WRN(log_AppMain, LOG_LEVEL, "Testing Base App");

    Azura::Modding::ModManager mm;
    bool result = mm.Load("../Mod1/Mod1", "Mod1");

    if (!result)
    {
      LOG_ERR(log_AppMain, LOG_LEVEL, "Can't Load Mod1 Library");
      throw;
    }

    const auto modPtr = mm.GetMod("Mod1");

    if (modPtr == nullptr)
    {
      LOG_ERR(log_AppMain, LOG_LEVEL, "Mod1 Ptr was null");
      throw;
    }

    const int op1 = 5;
    const int op2 = 5;

    const int mod1Data = reinterpret_cast<GameMod*>(modPtr)->DoMath(op1, op2); // NOLINT
    LOG_INF(log_AppMain, LOG_LEVEL, "Mod1 %d <op> %d was %d", op1, op2, mod1Data);
  }
  catch(const std::runtime_error& e)
  {
    LOG_ERR(log_AppMain, LOG_LEVEL, e.what());
    std::cout << e.what();
  }

  return 0;
}
