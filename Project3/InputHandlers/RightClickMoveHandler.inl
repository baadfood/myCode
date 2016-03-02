#pragma once

#include "../UserInputHandler.h"
#include <glm/glm.hpp>
#include <SDL.h>
#include "../Object.h"
#include "../CameraBase.h"

class MouseRightDragMoveHandler: public UserInputHandler
{
public:
  MouseRightDragMoveHandler(CameraBase * p_camera):
  m_camera(p_camera)
  {
  }
  ~MouseRightDragMoveHandler()
  {
  }
  virtual bool handleInput(SDL_Event const * sdlEvent)
  {
    if (sdlEvent->type == SDL_MOUSEBUTTONDOWN
        && sdlEvent->button.button == 3)
    {
      m_startPos = m_camera->screenToWorld(glm::i32vec2(sdlEvent->motion.x, sdlEvent->motion.y));
      return true;
    }
    else if (sdlEvent->type == SDL_MOUSEMOTION
             && sdlEvent->motion.state == 1)
    {
      glm::u64vec2 currentPos = m_camera->screenToWorld(glm::i32vec2(sdlEvent->motion.x, sdlEvent->motion.y));
      m_target->moveTo(currentPos - m_startPos);
      return true;
    }
    return false;
  }
  virtual void bindObject(Object * p_object)
  {
    m_target = p_object;
  }

private:
  Object * m_target;
  CameraBase * m_camera;
  glm::u64vec2 m_startPos;
};