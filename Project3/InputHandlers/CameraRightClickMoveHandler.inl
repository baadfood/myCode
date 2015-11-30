#pragma once

#include "../UserInputHandler.h"
#include <glm\glm.hpp>
#include <SDL.h>
#include "../Object.h"
#include "../CameraBase.h"

class CameraRightClickMoveHandler: public UserInputHandler
{
public:
  CameraRightClickMoveHandler(CameraBase * p_camera):
  m_camera(p_camera)
  {
  }
  ~CameraRightClickMoveHandler()
  {
  }
  virtual bool handleInput(SDL_Event const * sdlEvent)
  {
    if (sdlEvent->type == SDL_MOUSEBUTTONDOWN
        && sdlEvent->button.button == 3)
    {
      m_startCameraPos = m_camera->getPos();
      m_startPos = m_camera->screenToWorld(glm::i32vec2(sdlEvent->motion.x, sdlEvent->motion.y));
      return true;
    }
    else if (sdlEvent->type == SDL_MOUSEMOTION
             && sdlEvent->motion.state == 4)
    {
      glm::i64vec2 currentPos = m_camera->screenToWorld(glm::i32vec2(sdlEvent->motion.x, sdlEvent->motion.y));
      m_camera->moveTo((m_startCameraPos - currentPos) + m_startPos);
      m_startCameraPos = m_camera->getPos();
      return true;
    }
    return false;
  }
  virtual void bindObject(Object * p_object)
  {
    m_camera = dynamic_cast<CameraBase*>(p_object);
  }

private:
  CameraBase * m_camera;
  glm::i64vec2 m_startPos;
  glm::i64vec2 m_startCameraPos;
};