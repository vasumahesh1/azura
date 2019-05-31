#pragma once

#include <unordered_map>
#include <boost/function.hpp>


#include "Log/Log.h"
#include "Types.h"
#include "Mod.h"

#define MOD_CREATE_ALIAS Factory_CreateAlias

namespace Azura {
class Log;
}

namespace Azura {
namespace Modding {

  class ModManager
  {
    using mod_t = Mod*();

    struct ModEntry
    {
      String m_path;
      boost::function<mod_t> m_creator;
    };

  public:
    ModManager();
    ~ModManager();

    ModManager(const ModManager& other) = default;
    ModManager(ModManager&& other) = default;
    ModManager& operator=(const ModManager& other) = default;
    ModManager& operator=(ModManager&& other) = default;

    bool Load(const String& modPath, const String& name);
    void Unload(const String& name);
    Mod* GetMod(const String& name);

  private:
    Log m_log;

    std::unordered_map<String, Mod*> m_mods;
    std::unordered_map<String, ModEntry> m_modEntry;
  };

} // namespace Modding
} // namespace Azura
