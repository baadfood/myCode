#pragma once

#include <memory>

struct GameState;

class GameManager
{
public:
  GameManager();
  ~GameManager();

  virtual bool progressFrame();
  virtual void setGameState(GameState * p_state);
  virtual GameState * getGameState();

protected:

private:

  struct Private;
  std::unique_ptr<Private> d;

};

