#pragma once

#include "Manager.h"
#include <memory>
struct GameState;
union SDL_Event;

class InputManager : public Manager
{
public:
  InputManager();
  virtual ~InputManager();

  virtual void advance(GameState * p_state);
  virtual std::string const & getName() const;
  virtual void handleEvent(SDL_Event * p_event, GameState * p_state);

protected:
private:
  struct Private;
  std::unique_ptr<Private> d;
};

