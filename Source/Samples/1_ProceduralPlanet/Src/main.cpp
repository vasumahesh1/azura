#include "ProceduralPlanet/AppRenderer.h"
#include <iostream>
int main()
{
  try {
    Azura::AppRenderer app;
    app.Initialize();
    app.Run();
    app.Destroy();
  }
  catch(std::runtime_error e)
  {
    std::cout << e.what();
  }
  return 0;
}