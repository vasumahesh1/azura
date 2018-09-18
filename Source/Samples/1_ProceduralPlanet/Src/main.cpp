#include "ProceduralPlanet/AppRenderer.h"
#include "Log/Log.h"

int main()
{
  auto log_AppMain = Azura::Log("VulkanRenderSystem");

  try {
    Azura::AppRenderer app;
    app.Initialize();
    app.Run();
    app.Destroy();
  }
  catch(const std::runtime_error& e)
  {
    LOG_ERR(log_AppMain, LOG_LEVEL, e.what());
  }
  return 0;
}