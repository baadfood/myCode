#include "FpsManager.h"

#include "GameState.h"

#include <SDL.h>

struct FpsManager::Private
{
  Uint32 lastTicks;
  Uint32 frameDelay;
  std::string name;
};

FpsManager::FpsManager() :
  d(new Private())
{
  d->name = "FpsManager";
}

std::string const & FpsManager::getName() const
{
  return d->name;
}

FpsManager::~FpsManager()
{}

void FpsManager::advance(GameState * p_state)
{
  if (d->lastTicks + d->frameDelay > p_state->currentFrameTime)
  {
    SDL_Delay(d->lastTicks + d->frameDelay - p_state->prevFrameTime);
  }
  d->lastTicks = p_state->prevFrameTime;
}