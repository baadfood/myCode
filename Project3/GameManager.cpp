#include "GameManager.h"

#include <vector>
#include "Manager.h"
#include <SDL.h>

#include <iostream>
#include <sstream>

#include "GameState.h"

#include "InputManager.h"
#include "PhysicsManager.h"
#include "LogicManager.h"
#include "RenderManager.h"
#include "FpsManager.h"

#include "DebugPrint.h"

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
//  d->state->currentFrameTime += 30;
  d->state->currentFrameTime = SDL_GetTicks();
  d->state->ticksAdvanced = d->state->currentFrameTime - d->state->prevFrameTime;

  std::stringstream sstream;

//  sstream << "Frame start " << d->state->ticksAdvanced << '\n';
  for (auto iter = d->managers.begin();
  iter != d->managers.end();
    iter++)
  {
    Uint32 ticks = SDL_GetTicks();
    (*iter)->advance(d->state);
//	sstream << (*iter)->getName() << " ticks: " << SDL_GetTicks() - ticks << '\n';
  }
//  std::cout << sstream.str() << '\n';
  
  return true;
}
