#include "ProceduralPlanet/AppRenderer.h"

int main()
{
  Azura::AppRenderer app;
  app.Initialize();
  app.Run();
  app.Destroy();
  return 0;
}