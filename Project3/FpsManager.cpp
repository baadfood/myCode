#include "FpsManager.h"

#include "GameState.h"

#include <SDL.h>

#include "DebugPrint.h"

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
  uint32_t timeLeft = SDL_GetTicks() - p_state->currentFrameTime;
  if (timeLeft < d->frameDelay)
  {
    SDL_Delay(d->frameDelay - timeLeft);
  }
  else
  {
    std::cout << "Too much to compute to keep up with desired framerate : " << p_state->ticksAdvanced - d->frameDelay << std::endl;
  }
  d->lastTicks = p_state->prevFrameTime;
}