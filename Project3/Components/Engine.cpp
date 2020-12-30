#include "Components/Engine.h"
#include <Object.h>
#include <assert.h>

struct Engine::Private
{
  glm::u64 thrust = 0;
  glm::f64 orderedImpulse = 0;
  glm::u64 orderedThrust = 0;
};

Engine::Engine(glm::u64 p_thrust):
d(new Private)
{
  d->thrust = p_thrust;
}

Engine::~Engine()
{

}

void Engine::getEffectOnObject(glm::u64 p_nanos, float p_thrust, glm::i64vec2 & p_accel, float & p_rotAccel)
{
  glm::f64vec2 impulseScalar(0, double(p_nanos) / 1e9 * d->thrust * p_thrust);
  impulseScalar = getWorldTransform().applyRotation(impulseScalar);
  
  getObject()->calculateImpluseEffect(impulseScalar, getObject()->getTransform2d().applyRotation(getTransform2d().pos), p_accel, p_rotAccel);
}

void Engine::advance(glm::u64 p_nanos, Object* p_object)
{
  Component::advance(p_nanos, p_object);
  applyImpulse(p_nanos);
}

void Engine::applyImpulse(glm::u64 p_nanos)
{
  glm::u64 thrust = 0;
  if(d->orderedThrust != 0)
  {
    thrust = d->orderedThrust * ((double(p_nanos) / 1e9));
  }
  else
  {
    thrust = std::min(static_cast<glm::f64>(p_nanos) / 1e9 * d->thrust, d->orderedImpulse);
    d->orderedImpulse -= thrust;
  }

  glm::f32vec3 engineColor(1.0, 1.0, 1.0);

  engineColor *= (thrust / ((double(p_nanos) / 1e9))) / d->thrust;

  setSelfIllumination(engineColor);
  
  glm::f64vec2 impulseScalar(0, thrust);
  
  impulseScalar = getWorldTransform().applyRotation(impulseScalar);
  
  getObject()->applyImpulse(impulseScalar, getObject()->getTransform2d().applyRotation(getTransform2d().pos));
}

void Engine::setConstantThrustP(glm::float32 p_thrust)
{
  assert(p_thrust >= 0.0 && p_thrust <= 1.0);
  d->orderedThrust = d->thrust * p_thrust; 
  d->orderedImpulse = 0;
}
  
void Engine::setConstantThrust(glm::u64 p_thrust)
{
  d->orderedThrust = std::min(p_thrust, d->thrust);
  d->orderedImpulse = 0;
}

void Engine::setImpulse(glm::u64 p_impulse)
{
  d->orderedThrust = 0;
  d->orderedImpulse = p_impulse;
}

void Engine::updateLogic(glm::u64 p_nanos)
{
  Component::updateLogic(p_nanos);
}
