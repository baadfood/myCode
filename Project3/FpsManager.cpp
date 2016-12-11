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
  d->frameDelay = 16;
}

std::string const & FpsManager::getName() const
{
  return d->name;
}

FpsManager::~FpsManager()
{}

void FpsManager::advance(GameState * p_state)
{
  if (d->frameDelay >= p_state->ticksAdvanced)
  {
    SDL_Delay(d->frameDelay - p_state->ticksAdvanced);
  }
  else
  {
    std::cout << "Too much to compute to keep up with desired framerate : " << p_state->ticksAdvanced - d->frameDelay << std::endl;
  }
  d->lastTicks = p_state->prevFrameTime;
}