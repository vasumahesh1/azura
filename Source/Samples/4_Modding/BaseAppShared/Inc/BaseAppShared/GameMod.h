#pragma once
#include "ModFactory/Mod.h"

namespace MyApp {

  class GameMod : public Azura::Modding::Mod
  {
  public:
    GameMod() = default;

    virtual ~GameMod() = default;

    GameMod(const GameMod& other) = delete;
    GameMod(GameMod&& other) noexcept = delete;
    GameMod& operator=(const GameMod& other) = delete;
    GameMod& operator=(GameMod&& other) noexcept = delete;

    virtual int DoMath(int a, int b) const = 0;

    void OnLoad(const Azura::Log& log) override;
    void OnUnload(const Azura::Log& log) override;
  };
  
} // namespace MyApp