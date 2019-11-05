#include "FlyByWireAi.h"
#include <Object.h>

#include <Components/Engine.h>

#include "utils.h"

struct FlyByWireAi::Private
{
  Object* object = nullptr;
  std::vector<Engine*> engines[eEngineDirectionCount];

  glm::f32 desiredAngle;
  glm::i64vec2 desiredPos;
  glm::i64vec2 desiredSpeed;

  double forwardAccel = 0;
  double reverseAccel = 0;
  double leftAccel = 0;
  double rightAccel = 0;
  float clockWiseRate = 0;
  float counterWiseRate = 0;
};

FlyByWireAi::FlyByWireAi() :
  d(new Private())
{

}

FlyByWireAi::~FlyByWireAi()
{

}

void FlyByWireAi::calibrateEngines()
{
  Object* myObject = getObject();
  d->clockWiseRate = 0;
  for (Engine* engine : d->engines[eClockwiseTurn])
  {
    glm::i64vec2 throwAway;
    float temp = 0;
    engine->getEffectOnObject(1e9, 1.0, throwAway, temp);
    d->clockWiseRate += temp;
  }
  d->counterWiseRate = 0;
  for (Engine* engine : d->engines[eCounterCTurn])
  {
    glm::i64vec2 throwAway;
    float temp = 0;
    engine->getEffectOnObject(1e9, 1.0, throwAway, temp);
    d->counterWiseRate += temp;
  }

  d->forwardAccel = 0;
  for (Engine* engine : d->engines[eForward])
  {
    glm::i64vec2 linear;
    float angular = 0;
    engine->getEffectOnObject(1e9, 1.0, linear, angular);
    d->forwardAccel += myObject->getTransform2d().revertRotation(linear).y;
  }

  d->reverseAccel = 0;
  for (Engine* engine : d->engines[eBackward])
  {
    glm::i64vec2 linear;
    float angular = 0;
    engine->getEffectOnObject(1e9, 1.0, linear, angular);
    d->reverseAccel += myObject->getTransform2d().revertRotation(-linear).y;
  }

  d->leftAccel = 0;
  for (Engine* engine : d->engines[eLeft])
  {
    glm::i64vec2 linear;
    float angular = 0;
    engine->getEffectOnObject(1e9, 1.0, linear, angular);
    d->leftAccel += myObject->getTransform2d().revertRotation(linear).x;
  }
  d->rightAccel = 0;
  for (Engine* engine : d->engines[eRight])
  {
    glm::i64vec2 linear;
    float angular = 0;
    engine->getEffectOnObject(1e9, 1.0, linear, angular);
    d->rightAccel += myObject->getTransform2d().revertRotation(linear).x;
  }
}

void FlyByWireAi::addEngine(Engine* p_engine, EEngineDirection p_direction)
{
  d->engines[p_direction].push_back(p_engine);
}

void FlyByWireAi::updateLogic(glm::i64 p_nanos)
{
  calibrateEngines();

  auto rot = getObject()->getRot();

  glm::float32 angleDiff = d->desiredAngle - rot;
  glm::float32 rotSpeed = getObject()->getRotSpeed();
  bool turnClockwise = true;
  glm::float32 thrust = 0;

  if (angleDiff == 0)
  {
    return;
  }

  double seconds = double(p_nanos) / 1e9;
  angleDiff -= rotSpeed * seconds;
  mika::fixAngle(angleDiff);
  double damping = 1.0;
  if (abs(angleDiff) < 0.1)
  {
    damping = abs(angleDiff) / 0.1;
  }

  if (angleDiff < 0)
  {
    float desiredSpeed = -std::sqrt(-2 * angleDiff / d->counterWiseRate) * d->counterWiseRate * damping; // TODO get rid of square root
    double desiredAccel = (desiredSpeed - rotSpeed) / seconds;
    if (desiredAccel > 0)
    {
      turnClockwise = false;
      thrust = std::min(1.0, desiredAccel / d->counterWiseRate);
    }
    else
    {
      turnClockwise = true;
      thrust = std::min(1.0, desiredAccel / d->clockWiseRate);
    }
  }
  else
  {
    float desiredSpeed = std::sqrt(2 * angleDiff / -d->clockWiseRate) * -d->clockWiseRate * damping; // TODO get rid of square root
    double desiredAccel = (desiredSpeed - rotSpeed) / seconds;
    if (desiredAccel > 0)
    {
      turnClockwise = false;
      thrust = std::min(1.0, desiredAccel / d->counterWiseRate);
      //        std::cout << "Next speed should be " << rotSpeed + d->counterWiseRate * thrust * ((double(p_nanos) / 1e9)) << std::endl;
    }
    else
    {
      turnClockwise = true;
      thrust = std::min(1.0, desiredAccel / d->clockWiseRate);
      //        std::cout << "Next speed should be " << rotSpeed + d->clockWiseRate * thrust * ((double(p_nanos) / 1e9)) << std::endl;
    }
  }
  //    std::cout << "Thrust " << thrust << std::endl;

  if (turnClockwise)
  {
    for (Engine* engine : d->engines[eCounterCTurn])
    {
      engine->setConstantThrustP(0.0);
    }
    for (Engine* engine : d->engines[eClockwiseTurn])
    {
      engine->setConstantThrustP(thrust);
    }
  }
  else
  {
    for (Engine* engine : d->engines[eCounterCTurn])
    {
      engine->setConstantThrustP(thrust);
    }
    for (Engine* engine : d->engines[eClockwiseTurn])
    {
      engine->setConstantThrustP(0.0);
    }
  }
}

void FlyByWireAi::setDesiredDirection(glm::f32 p_direction)
{
  d->desiredAngle = p_direction;
}

void FlyByWireAi::setDesiredLocation(glm::i64vec2 const& p_position)
{
  d->desiredPos = p_position;
}

void FlyByWireAi::setDesiredSpeedAtLocation(glm::i64vec2 const& p_speed)
{
  d->desiredSpeed = p_speed;
}

void FlyByWireAi::setDesiredState(glm::f32 p_direction, glm::i64vec2 const& p_position, glm::i64vec2 const& p_speed)
{
  setDesiredDirection(p_direction);
  setDesiredLocation(p_position);
  setDesiredSpeedAtLocation(p_speed);
}


