#pragma once
#include "BaseAppShared/GameMod.h"
#include "ModFactory/ModManager.h"

#include <boost/dll/alias.hpp>

namespace Mod1 {

  class Modification1 : public MyApp::GameMod
  {

  public:
    virtual Azura::String GetName() override;
    virtual int DoMath(int a, int b) const override;

    REGISTER_MOD(Modification1);
  };
  
} // namespace Mod1

BOOST_DLL_ALIAS(
  Mod1::Modification1::Create,    // <-- this function is exported with...
  Factory_CreateAlias             // <-- ...this alias name
)