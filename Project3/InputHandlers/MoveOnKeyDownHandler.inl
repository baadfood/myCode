#pragma once

#include "../UserInputHandler.h"
#include <glm\glm.hpp>
#include <SDL.h>
#include "../Object.h"

class MoveOnKeyDownHandler: public UserInputHandler
{
public:
  MoveOnKeyDownHandler(Uint32 p_keyToMoveOn, Uint32 p_keyMod , glm::u64vec2 p_moveBy):
  m_keyCode(p_keyToMoveOn),
  m_keyMod(p_keyMod),
  m_offset(p_moveBy)
  {

  }
  ~MoveOnKeyDownHandler()
  {

  }
  virtual bool handleInput(SDL_Event const * sdlEvent)
  {
    if(sdlEvent->type == SDL_KEYDOWN
    && sdlEvent->key.keysym.mod == m_keyMod
    && sdlEvent->key.keysym.scancode == m_keyCode
    && sdlEvent->key.repeat == 0
    && m_target)
    {
      m_target->moveBy(m_offset);
    }
  }
  virtual void bindObject(Object * p_object)
  {
    m_target = p_object;
  }

private:
  Object * m_target;
  glm::u64vec2 m_offset;
  Uint32 m_keyCode;
  Uint32 m_keyMod;
};