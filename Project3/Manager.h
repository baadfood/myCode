#pragma once

#include <string>

struct GameState;

class Manager
{
public:
  virtual void advance(GameState * p_state) = 0;
  virtual std::string const & getName() const = 0;
};

