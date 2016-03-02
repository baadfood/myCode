#include "InputManager.h"

#include "GameState.h"
#include "SpatialTree.h"

#include "Object.h"

#include "CameraBase.h"
#include "Display.h"
#include "QuadTree.h"

#include <unordered_set>

struct InputManager::Private
{
  Uint32 maxTicksToHandleInputFor;
  SDL_Event event;
  Uint32 lastTicks;
  std::string name;
};

InputManager::InputManager() :
  d(new Private())
{
  d->maxTicksToHandleInputFor = 10;
  d->name = "InputManager";
}

InputManager::~InputManager()
{
}

std::string const & InputManager::getName() const
{
  return d->name;
}

void InputManager::advance(GameState * p_state)
{
  int eventsHandled = 0;
  Uint32 ticks = SDL_GetTicks();
    // Max time to handle inputs?
  while (SDL_PollEvent(&d->event))
  {
    if (SDL_GetTicks() - d->maxTicksToHandleInputFor > ticks)
    {
      std::cerr << "Too much input to handle, need to stop : " << eventsHandled << std::endl;
      // Too much input to handle, let's start doing other stuff so things don't lag
      return;
    }
    if (d->event.type == SDL_KEYDOWN)
    {
      p_state->keysHeldDown.insert(d->event.key.keysym);
    }
    else if (d->event.type == SDL_KEYUP)
    {
      p_state->keysHeldDown.erase(d->event.key.keysym);
    }
    handleEvent(&(d->event), p_state);
    eventsHandled++;
  }
}

void InputManager::handleEvent(SDL_Event * p_event, GameState * p_state)
{
  for(auto iter = p_state->displays.begin();
  iter != p_state->displays.end();
    iter++)
  {
    if((*iter)->getWinId() == p_event->window.windowID)
    {
      if((*iter)->handleEvent(p_event, p_state) == false)
      {
        for(auto appGlobalHanlerIter = p_state->globalHandlers.begin();
        appGlobalHanlerIter != p_state->globalHandlers.end();
          appGlobalHanlerIter++)
        {
          if((*appGlobalHanlerIter)->handleInput(p_event))
          {
            break;
          }
        }
      }
      // Totally unhandled event.
      break;
    }
  }
}