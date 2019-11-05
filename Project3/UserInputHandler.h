#pragma once

#include <SDL.h>
#include <GL/glew.h>

class Object;

struct Event
{
  Event(SDL_Event & p_event, glm::i64vec2& p_worldPos) :
    sdlEvent(p_event), worldPos(p_worldPos)
  {}
  SDL_Event& sdlEvent;
  glm::i64vec2& worldPos;
};

class UserInputHandler
{
public:
  // Return true if input consumed
  virtual bool handleInput(Event const & p_event) = 0;
};