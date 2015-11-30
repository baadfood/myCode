#pragma once

#include <SDL.h>

class Object;

class UserInputHandler
{
public:
  // Return true if input consumed
  virtual bool handleInput(SDL_Event const * sdlEvent) = 0;
  virtual void bindObject(Object * p_object) = 0;
};