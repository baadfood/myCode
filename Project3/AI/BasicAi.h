#pragma once

#include <AI/Ai.h>
#include <Components/Sensor.h>
#include <memory>

class BasicAi: public Ai
{
public:
  BasicAi();
  ~BasicAi();
  
  void addSensor(Sensor * p_sensor);
  
  virtual void updateLogic(glm::i64 p_nanos);
  
private:
  struct Private;
  std::unique_ptr<Private> d;
};