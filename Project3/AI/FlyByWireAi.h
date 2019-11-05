#pragma once

#include <AI/Ai.h>
#include <UserInputHandler.h>
#include <memory>

class Engine;
class FlyByWireAi : public Ai
{
public:
  enum EEngineDirection
  {
    eClockwiseTurn = 0,
    eCounterCTurn,
    eForward,
    eBackward,
    eLeft,
    eRight,
    eEngineDirectionCount
  };

  FlyByWireAi();
  ~FlyByWireAi();

  void addEngine(Engine* p_engine, EEngineDirection);

  virtual void updateLogic(glm::i64 p_nanos);

  void setDesiredDirection(glm::f32 p_direction);
  void setDesiredLocation(glm::i64vec2 const& p_position);
  void setDesiredSpeedAtLocation(glm::i64vec2 const& p_speed);

  void setDesiredState(glm::f32 p_direction,
                       glm::i64vec2 const & p_position,
                       glm::i64vec2 const & p_speed);

private:
  virtual void calibrateEngines();
  struct Private;
  std::unique_ptr<Private> d;
};