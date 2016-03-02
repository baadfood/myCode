#include "GameManager.h"

#include <vector>
#include "Manager.h"
#include <SDL.h>

#include <iostream>

#include "GameState.h"

#include "InputManager.h"
#include "PhysicsManager.h"
#include "LogicManager.h"
#include "RenderManager.h"
#include "FpsManager.h"


struct GameManager::Private
{
  GameState * state;
  std::vector<std::unique_ptr<Manager>> managers;
};

GameManager::GameManager():
d(new Private())
{
  d->managers.emplace_back(new PhysicsManager());
  d->managers.emplace_back(new InputManager());
  d->managers.emplace_back(new LogicManager());
  d->managers.emplace_back(new FpsManager());
  d->managers.emplace_back(new RenderManager());
}

GameManager::~GameManager()
{
}

void GameManager::setGameState(GameState * p_state)
{
  d->state = p_state;
}

GameState * GameManager::getGameState()
{
  return d->state;
}

bool GameManager::progressFrame()
{
  d->state->prevFrameTime = d->state->currentFrameTime;
  d->state->currentFrameTime = SDL_GetTicks();
  d->state->ticksAdvanced = d->state->currentFrameTime - d->state->prevFrameTime;

  for (auto iter = d->managers.begin();
  iter != d->managers.end();
    iter++)
  {
    Uint32 ticks = SDL_GetTicks();
    (*iter)->advance(d->state);
    std::cout << (*iter)->getName() << " ticks: " << SDL_GetTicks() - ticks << std::endl;
  }
  return true;
}
