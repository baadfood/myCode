#pragma once

#include <AI/Ai.h>
#include <memory>

class Sensor;
class Engine;
class Component;
class BasicAi: public Ai
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
  
  BasicAi();
  ~BasicAi();
  
  void setTargettingReticule(Component * p_reticule);
  void addSensor(Sensor * p_sensor);
  void addEngine(Engine * p_engine, EEngineDirection);
  
  virtual void updateLogic(glm::i64 p_nanos);
  
private:
  struct Private;
  std::unique_ptr<Private> d;
};