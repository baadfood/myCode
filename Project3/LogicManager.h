#pragma once

#include "Manager.h"
#include <memory>
struct GameState;

class LogicManager : public Manager
{
public:
  LogicManager();
  virtual ~LogicManager();
  virtual std::string const & getName() const;
  virtual void advance(GameState * p_state);

protected:
private:
  struct Private;
  std::unique_ptr<Private> d;
};

