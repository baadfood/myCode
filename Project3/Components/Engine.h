#pragma once
#include "Component.h"

class Engine: public Component
{
public:
  Engine(glm::u64 p_thrust);
  ~Engine();
  
  virtual void updateLogic(glm::u64 p_nanos);

  void applyImpulse(glm::u64 p_nanos);
  
  virtual void advance(glm::u64 p_nanos, Object * p_object);

  void getEffectOnObject(glm::u64 p_nanos, float p_thrust, glm::i64vec2 & p_accel, float & p_rotAccel);
  void setConstantThrustP(glm::float32 p_thrust);
  void setConstantThrust(glm::u64 p_thrust);
  void setImpulse(glm::u64 p_impulse);
  
private:
  struct Private;
  std::unique_ptr<Private> d;
};