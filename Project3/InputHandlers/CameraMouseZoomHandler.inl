#pragma once

#include "../UserInputHandler.h"
#include <glm\glm.hpp>
#include <SDL.h>
#include "../Object.h"
#include "../CameraWorldBased.h"

class CameraMouseZoomHandler: public UserInputHandler
{
public:
  CameraMouseZoomHandler(CameraWorldBased * p_camera):
    m_camera(p_camera)
  {}
  ~CameraMouseZoomHandler()
  {}
  virtual bool handleInput(SDL_Event const * sdlEvent)
  {
    if(sdlEvent->type == SDL_MOUSEWHEEL)
    {
      m_camera->setWorldPerPixel(m_camera->getWorldPerPixel() + (m_camera->getWorldPerPixel() * sdlEvent->wheel.y) / 5);
      return true;
    }
    return false;
  }
  virtual void bindObject(Object * p_object)
  {
    m_camera = dynamic_cast<CameraWorldBased*>(p_object);
  }

private:
  CameraWorldBased * m_camera;
  glm::u64vec2 m_startPos;
};