#include "ModFactory/ModManager.h"
#include <boost/dll/import.hpp>

namespace Azura {
namespace Modding {

namespace dll = boost::dll;

ModManager::ModManager()
  : m_log(Log("ModManager")) {
}

ModManager::~ModManager() {
  for (auto& modPair : m_mods) {
    modPair.second->OnUnload(m_log);
    delete modPair.second; // NOLINT
  }
}

bool ModManager::Load(const String& modPath, const String& name) {

  // Already exists
  if (m_mods.find(name) != m_mods.end()) {
    LOG_WRN(m_log, LOG_LEVEL, "Tried to load a mod that was loaded: %s", name.c_str());
    return false;
  }

  try {
    LOG_DBG(m_log, LOG_LEVEL, "Trying to load mod: %s", name.c_str());

    // type of imported symbol must be explicitly specified
    const boost::function<mod_t> createFunc = boost::dll::import_alias<mod_t>(
                                                                              modPath,          // path to library
                                                                              "Factory_CreateAlias", // symbol to import
                                                                              dll::load_mode::append_decorations
                                                                              // do append extensions and prefixes
                                                                             );

    m_mods[name] = createFunc();
    if (m_mods[name] == nullptr) {
      LOG_WRN(m_log, LOG_LEVEL, "Mod create function returned nothing: %s", name.c_str());
      return false;
    }

    m_modEntry[name] = ModEntry{modPath, createFunc};

    LOG_DBG(m_log, LOG_LEVEL, "Mod Created Successfully: %s", name.c_str());

    m_mods[name]->OnLoad(m_log);

    LOG_DBG(m_log, LOG_LEVEL, "Mod Loaded Successfully: %s", name.c_str());

    return true;
  }
  catch(const std::runtime_error& e)
  {
    LOG_FTL(m_log, LOG_LEVEL, "Fatal error loading mod: %s", e.what());
    return false;
  }
}

void ModManager::Unload(const String& name) {
  // Tried to unload something that doesn't exist
  if (m_mods.find(name) == m_mods.end()) {
    LOG_WRN(m_log, LOG_LEVEL, "Tried to unload a Mod that wasn't loaded: %s", name.c_str());
    return;
  }

  LOG_DBG(m_log, LOG_LEVEL, "Mod Unloaded Successfully: %s", name.c_str());

  m_mods[name]->OnUnload(m_log);
  delete m_mods[name]; // NOLINT
  m_mods.erase(name);
  m_modEntry.erase(name);
}

Mod* ModManager::GetMod(const String& name) {
  if (m_mods.find(name) == m_mods.end()) {
    LOG_WRN(m_log, LOG_LEVEL, "Tried to get a Mod that wasn't loaded: %s", name.c_str());
    return nullptr;
  }

  return m_mods[name];
}
} // namespace Modding
} // namespace Azura
