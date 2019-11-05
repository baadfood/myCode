#pragma once

#include "../UserInputHandler.h"
#include <glm/glm.hpp>
#include <SDL.h>
#include "../Object.h"
#include "../CameraBase.h"
#include "AI/FlyByWireAi.h"

class FlyByWireAiController : public UserInputHandler
{
public:
  FlyByWireAiController(FlyByWireAi* p_ai) :
    m_ai(p_ai)
  {
  }
  ~FlyByWireAiController()
  {
  }
  bool handleInput(Event const& p_event)
  {
    glm::i64vec2 pos = m_ai->getObject()->getPos();

    if (p_event.sdlEvent.type != SDL_MOUSEBUTTONDOWN ||
        p_event.sdlEvent.button.button != SDL_BUTTON_RIGHT)
    {
      return false;
    }

    float angleToAimAt = atan2(-(p_event.worldPos.x - pos.x), p_event.worldPos.y - pos.y);

    m_ai->setDesiredDirection(angleToAimAt);

    return true;

  }

private:
  Object* m_target;
  FlyByWireAi* m_ai;
  glm::u64vec2 m_startPos;
};