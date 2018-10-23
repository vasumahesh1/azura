#include "Log/Log.h"

#include <iostream>
#include "App/App.h"

int main()
{
  auto log_AppMain = Azura::Log("DeferredRenderer");

  try {
    Azura::App app;
    app.Initialize();
    app.Run();
    app.CleanUp();
  }
  catch(const std::runtime_error& e)
  {
    LOG_ERR(log_AppMain, LOG_LEVEL, e.what());
    std::cout << e.what();
  }
  return 0;
}
