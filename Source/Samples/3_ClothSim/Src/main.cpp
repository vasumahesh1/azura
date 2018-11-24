#include "ClothSim/AppRenderer.h"
#include "Log/Log.h"

#include <iostream>

int main()
{
  auto log_AppMain = Azura::Log("ClothSim");

  try {
    Azura::AppRenderer app;
    app.Initialize();
    app.Run();
    app.Destroy();
  }
  catch(const std::runtime_error& e)
  {
    LOG_ERR(log_AppMain, LOG_LEVEL, e.what());
    std::cout << e.what();
  }
  return 0;
}