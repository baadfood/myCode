#include "FlyByWireAi.h"
#include <Object.h>

#include <Components/Engine.h>

#include "utils.h"

struct FlyByWireAi::Private
{
  Object* object = nullptr;
  Object* target = nullptr;
  std::vector<Engine*> engines[eEngineDirectionCount];

  glm::f32 desiredAngle = 0.0f;
  glm::f32 desiredRotationSpeed = 0.0f;
  glm::i64vec2 desiredPos = glm::i64vec2(0, 0);
  glm::i64vec2 positionOffset = glm::i64vec2(0,0);
  glm::i64vec2 desiredSpeed = glm::i64vec2(0, 0);

  double forwardAccel = 0;
  double reverseAccel = 0;
  double leftAccel = 0;
  double rightAccel = 0;
  float clockWiseRate = 0;
  float counterWiseRate = 0;
  glm::f32 maxSpeedPercentage = 1.0;
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
    d->reverseAccel += myObject->getTransform2d().revertRotation(linear).y;
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

void FlyByWireAi::handleRotation(glm::i64 p_nanos)
{
  auto rot = getObject()->getRot();
  auto pos = getObject()->getPos();

  double angleDiff = d->desiredAngle - rot;
  glm::float32 rotSpeed = getObject()->getRotSpeed();
  bool turnClockwise = true;
  glm::float32 thrust = 0;

  if (angleDiff == 0)
  {
    return;
  }

  double seconds = double(p_nanos) / 1e9;
  mika::fixAngle(angleDiff);

  auto projectedPoint = angleDiff - rotSpeed * seconds;
  auto projectedCAccel = angleDiff - (rotSpeed + d->clockWiseRate * seconds) * seconds;
  auto projectedCCAccel = angleDiff - (rotSpeed + d->counterWiseRate * seconds) * seconds;

  float desiredSpeed;

  float maxUsableAccel = 2.0 * d->maxSpeedPercentage;

  if (projectedPoint < 0 && projectedCAccel > 0
      || projectedPoint > 0 && projectedCCAccel < 0)
  {
    desiredSpeed = angleDiff / seconds;
  }
  else
  {
    if (projectedPoint < 0)
    {
      desiredSpeed = -std::sqrt(std::abs(maxUsableAccel * projectedCCAccel * d->counterWiseRate));
    }
    else
    {
      desiredSpeed = std::sqrt(std::abs(maxUsableAccel * projectedCAccel * d->clockWiseRate));
    }
  }

  desiredSpeed += d->desiredRotationSpeed;

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

void FlyByWireAi::setTarget(Object* p_target)
{
  d->target = p_target;
}

void FlyByWireAi::handleLocation(glm::i64 p_nanos)
{
  auto myPos = getObject()->getPos();

  auto distance = d->desiredPos - myPos;
  auto speed = getObject()->getSpeed();

  double seconds = double(p_nanos) / 1e9;

  auto rot = getObject()->getRot();
  distance = getObject()->getTransform2d().revertRotation(distance);

  

  if (distance.x + distance.y == 0)
  {
    return;
  }

  double angleDiff = d->desiredAngle - rot;

  double desiredSpeed = 0;

  auto mySpeed = getObject()->getTransform2d().revertRotation(speed);

  auto myDesiredSpeedAtLocation = getObject()->getTransform2d().revertRotation(d->desiredSpeed);

  glm::fvec2 desiredAccel;

  glm::f64vec2 thrust;

  int mode = 0;
  double maxUsableAccel = 2.0 * d->maxSpeedPercentage;



  glm::fvec2 projectedPoint(distance.x + mySpeed.x * seconds, distance.y + mySpeed.y * seconds);
  glm::fvec2 projectedAccel(distance.x + (mySpeed.x + d->rightAccel * seconds) * seconds, distance.y + (mySpeed.y + d->forwardAccel * seconds) * seconds);
  glm::fvec2 projectedDecel(distance.x + (mySpeed.x + d->leftAccel * seconds) * seconds, distance.y + (mySpeed.y + d->reverseAccel * seconds) * seconds);

  if (std::abs(angleDiff) > 0.01)
  {
    double accel = std::min(-d->reverseAccel, std::min(d->forwardAccel, std::min(-d->leftAccel, d->rightAccel)));

    accel *= maxUsableAccel;

    if (distance.y > 0)
    {
      desiredSpeed = std::sqrt(distance.y * accel);
    }
    else
    {
      desiredSpeed = -std::sqrt(-distance.y * accel);
    }

    desiredSpeed += myDesiredSpeedAtLocation.y;

    desiredAccel.y = (desiredSpeed - mySpeed.y) / seconds;

    if (distance.x > 0)
    {
      desiredSpeed = std::sqrt(distance.x * accel);
    }
    else
    {
      desiredSpeed = -std::sqrt(-distance.x * accel);
    }
    desiredSpeed += myDesiredSpeedAtLocation.x;
    desiredAccel.x = (desiredSpeed - mySpeed.x) / seconds;

    if (desiredAccel.y > 0)
    {
      thrust.y = desiredAccel.y / d->forwardAccel;
    }
    else
    {
      thrust.y = desiredAccel.y / -d->reverseAccel;
    }
    if (desiredAccel.x > 0)
    {
      thrust.x = desiredAccel.x / d->rightAccel;
    }
    else
    {
      thrust.x = desiredAccel.x / -d->leftAccel;
    }
    if (std::abs(thrust.x) > std::abs(thrust.y))
    {
      thrust /= std::abs(thrust.x);
    }
    else
    {
      thrust /= std::abs(thrust.y);
    }
  }
  else
  {
    if (distance.y > 0)
    {
      desiredSpeed = std::sqrt(maxUsableAccel * distance.y * -d->reverseAccel);
    }
    else
    {
      desiredSpeed = -std::sqrt(maxUsableAccel * -distance.y * d->forwardAccel);
    }
    desiredSpeed += myDesiredSpeedAtLocation.y;
    desiredAccel.y = (desiredSpeed - mySpeed.y) / seconds;

    if (distance.x > 0)
    {
      desiredSpeed = std::sqrt(maxUsableAccel * distance.x * -d->leftAccel);
    }
    else
    {
      desiredSpeed = -std::sqrt(maxUsableAccel * -distance.x * d->rightAccel);
    }
    desiredSpeed += myDesiredSpeedAtLocation.x;
    desiredAccel.x = (desiredSpeed - mySpeed.x) / seconds;
    if (desiredAccel.y > 0)
    {
      thrust.y = std::min(1.0, desiredAccel.y / d->forwardAccel);
    }
    else
    {
      thrust.y = -std::min(1.0, desiredAccel.y / d->reverseAccel);
    }
    if (desiredAccel.x > 0)
    {
      thrust.x = std::min(1.0, desiredAccel.x / d->rightAccel);
    }
    else
    {
      thrust.x = -std::min(1.0, desiredAccel.x / d->leftAccel);
    }
  }

  if (projectedPoint.x < 0 && projectedAccel.x > 0
      || projectedPoint.x > 0 && projectedDecel.x < 0)
  {
    desiredAccel.x = ((distance.x / seconds) - mySpeed.x + myDesiredSpeedAtLocation.x) / seconds;
    if (desiredAccel.x > 0)
    {
      thrust.x = std::min(1.0, desiredAccel.x / d->rightAccel);
    }
    else
    {
      thrust.x = std::max(-1.0, desiredAccel.x / -d->leftAccel);
    }
  }
  if (projectedPoint.y < 0 && projectedAccel.y > 0
      || projectedPoint.y > 0 && projectedDecel.y < 0)
  {
    desiredAccel.y = ((distance.y / seconds) - mySpeed.y + myDesiredSpeedAtLocation.y) / seconds;
    if (desiredAccel.y > 0)
    {
      thrust.y = std::min(1.0, desiredAccel.y / d->forwardAccel);
    }
    else
    {
      thrust.y = std::max(-1.0, desiredAccel.y / -d->reverseAccel);
    }
  }

  if (thrust.y > 0)
  {
    for (Engine* engine : d->engines[eBackward])
    {
      engine->setConstantThrustP(0.0);
    }
    for (Engine* engine : d->engines[eForward])
    {
      engine->setConstantThrustP(thrust.y);
    }
  }
  else
  {
    for (Engine* engine : d->engines[eBackward])
    {
      engine->setConstantThrustP(-thrust.y);
    }
    for (Engine* engine : d->engines[eForward])
    {
      engine->setConstantThrustP(0.0);
    }
  }

  if (thrust.x > 0)
  {
    for (Engine* engine : d->engines[eLeft])
    {
      engine->setConstantThrustP(0.0);
    }
    for (Engine* engine : d->engines[eRight])
    {
      engine->setConstantThrustP(thrust.x);
    }
  }
  else
  {
    for (Engine* engine : d->engines[eLeft])
    {
      engine->setConstantThrustP(-thrust.x);
    }
    for (Engine* engine : d->engines[eRight])
    {
      engine->setConstantThrustP(0.0);
    }
  }

/*  std::vector<double> values;
  static double timeNow = 0;
  timeNow += seconds;
  values.push_back(timeNow);
  values.push_back(mode);
  values.push_back(double(myPos.x) / OBJTOWORLD);
  values.push_back(double(d->desiredPos.x) / OBJTOWORLD);
  values.push_back(double(distance.x) / OBJTOWORLD);
  values.push_back(double(mySpeed.x) / OBJTOWORLD);
  values.push_back(desiredAccel.x / OBJTOWORLD);
  values.push_back(projectedPoint.x);
  values.push_back(projectedAccel.x);
  values.push_back(projectedDecel.x);
  values.push_back(projectedPoint.y);
  values.push_back(projectedAccel.y);
  values.push_back(projectedDecel.y);
  values.push_back(thrust.x);
  values.push_back(angleDiff);

  std::ios_base::fmtflags f(std::cout.flags());

  std::cout.setf(std::ios::fixed);
  std::cout.setf(std::ios::showpoint);
  std::cout.precision(8);
  std::cout << "Data: ";
  for (auto value : values)
  {
    std::cout << (value > 0 ? " " : "") << value << ", ";
  }
  std::cout << "\n";

  std::cout.flags(f);
  */
}

//void FlyByWireAi::handleLocation(glm::i64 p_nanos)
//{
//  auto myPos = getObject()->getPos();
//
//  auto distance = d->desiredPos - myPos;
//  auto speed = getObject()->getSpeed();
//
//  double seconds = double(p_nanos) / 1e9;
//
//  auto rot = getObject()->getRot();
//  distance = getObject()->getTransform2d().revertRotation(distance);
//
//  if (distance.x + distance.y == 0)
//  {
//    return;
//  }
//
//  double angleDiff = d->desiredAngle - rot;
//
//  double desiredSpeed = 0;
//
//  auto mySpeed = getObject()->getTransform2d().revertRotation(speed);
//
//  glm::fvec2 desiredAccel;
//
//  glm::f64vec2 thrust;
//
//  glm::fvec2 projectedPoint(distance.x + mySpeed.x * seconds, distance.y + mySpeed.y * seconds);
//  glm::fvec2 projectedAccel(distance.x + (mySpeed.x + d->leftAccel * seconds) * seconds, distance.y + (mySpeed.y + d->forwardAccel * seconds) * seconds);
//  glm::fvec2 projectedDecel(distance.x + (mySpeed.x + d->rightAccel * seconds) * seconds, distance.y + (mySpeed.y + d->reverseAccel * seconds) * seconds);
//
//  if (projectedPoint.x < 0 && projectedAccel.x > 0
//  ||  projectedPoint.x > 0 && projectedDecel.x < 0
//  &&  projectedPoint.y < 0 && projectedAccel.y > 0
//  ||  projectedPoint.y > 0 && projectedDecel.y < 0)
//  {
//    desiredAccel.y = ((distance.y / seconds) - mySpeed.y) / seconds;
//    desiredAccel.x = ((distance.x / seconds) - mySpeed.x) / seconds;
//    if (desiredAccel.y > 0)
//    {
//      thrust.y = desiredAccel.y / d->forwardAccel;
//    }
//    else
//    {
//      thrust.y = desiredAccel.y / -d->reverseAccel;
//    }
//    if (desiredAccel.x > 0)
//    {
//      thrust.x = desiredAccel.x / d->rightAccel;
//    }
//    else
//    {
//      thrust.x = desiredAccel.x / -d->leftAccel;
//    }
//  }
//  else if (std::abs(angleDiff) > 0.01)
//  {
//
//    double accel = std::min(-d->reverseAccel, std::min(d->forwardAccel, std::min(-d->leftAccel, d->rightAccel)));
//
//    if (distance.y > 0)
//    {
//      desiredSpeed = std::sqrt(1.23 * distance.y * accel);
//    }
//    else
//    {
//      desiredSpeed = -std::sqrt(1.23 * -distance.y * accel);
//    }
//    desiredAccel.y = (desiredSpeed - mySpeed.y) / seconds;
//
//    if (distance.x > 0)
//    {
//      desiredSpeed = std::sqrt(1.23 * distance.x * accel);
//    }
//    else
//    {
//      desiredSpeed = -std::sqrt(1.23 * -distance.x * accel);
//    }
//    desiredAccel.x = (desiredSpeed - mySpeed.x) / seconds;
//
//    if (desiredAccel.y > 0)
//    {
//      thrust.y = desiredAccel.y / d->forwardAccel;
//    }
//    else
//    {
//      thrust.y = desiredAccel.y / -d->reverseAccel;
//    }
//    if (desiredAccel.x > 0)
//    {
//      thrust.x = desiredAccel.x / d->rightAccel;
//    }
//    else
//    {
//      thrust.x = desiredAccel.x / -d->leftAccel;
//    }
//
//    if (std::abs(thrust.x) > std::abs(thrust.y))
//    {
//      thrust /= std::abs(thrust.x);
//    }
//    else
//    {
//      thrust /= std::abs(thrust.y);
//    }
//  }
//  else
//  {
//      if (distance.y > 0)
//      {
//        desiredSpeed = std::sqrt(1.23 * distance.y * -d->reverseAccel);
//      }
//      else
//      {
//        desiredSpeed = -std::sqrt(1.23 * -distance.y * d->forwardAccel);
//      }
//      desiredAccel.y = (desiredSpeed - mySpeed.y) / seconds;
//    
//      if (distance.x > 0)
//      {
//        desiredSpeed = std::sqrt(1.23 * distance.x * -d->leftAccel);
//      }
//      else
//      {
//        desiredSpeed = -std::sqrt(1.23 * -distance.x * d->rightAccel);
//      }
//      desiredAccel.x = (desiredSpeed - mySpeed.x) / seconds;
//      if (desiredAccel.y > 0)
//      {
//        thrust.y = std::min(1.0, desiredAccel.y / d->forwardAccel);
//      }
//      else
//      {
//        thrust.y = -std::min(1.0, desiredAccel.y / d->reverseAccel);
//      }
//      if (desiredAccel.x > 0)
//      {
//        thrust.x = std::min(1.0, desiredAccel.x / d->rightAccel);
//      }
//      else
//      {
//        thrust.x = -std::min(1.0, desiredAccel.x / d->leftAccel);
//      }
//  }
//
//  if (thrust.y > 0)
//  {
//    for (Engine* engine : d->engines[eBackward])
//    {
//      engine->setConstantThrustP(0.0);
//    }
//    for (Engine* engine : d->engines[eForward])
//    {
//      engine->setConstantThrustP(thrust.y);
//    }
//  }
//  else
//  {
//    for (Engine* engine : d->engines[eBackward])
//    {
//      engine->setConstantThrustP(-thrust.y);
//    }
//    for (Engine* engine : d->engines[eForward])
//    {
//      engine->setConstantThrustP(0.0);
//    }
//  }
//
//  if (thrust.x > 0)
//  {
//    for (Engine* engine : d->engines[eLeft])
//    {
//      engine->setConstantThrustP(0.0);
//    }
//    for (Engine* engine : d->engines[eRight])
//    {
//      engine->setConstantThrustP(thrust.x);
//    }
//  }
//  else
//  {
//    for (Engine* engine : d->engines[eLeft])
//    {
//      engine->setConstantThrustP(-thrust.x);
//    }
//    for (Engine* engine : d->engines[eRight])
//    {
//      engine->setConstantThrustP(0.0);
//    }
//  }
//
//  std::vector<double> values;
//  static double timeNow = 0;
//  timeNow += seconds;
//  values.push_back(timeNow);
//  values.push_back(distance.x);
//  values.push_back(distance.y);
//  values.push_back(mySpeed.x);
//  values.push_back(mySpeed.y);
//  values.push_back(desiredAccel.x);
//  values.push_back(desiredAccel.y);
//  values.push_back(thrust.x);
//  values.push_back(thrust.y);
//
//  std::cout << "Data: ";
//  for (auto value : values)
//  {
//    std::cout << value << ", ";
//  }
//  std::cout << "\n";
//
//}

//void FlyByWireAi::handleLocation(glm::i64 p_nanos)
//{
//  auto myPos = getObject()->getPos();
//
//  auto distance = d->desiredPos - myPos;
//  auto speed = getObject()->getSpeed();
//
//  double seconds = double(p_nanos) / 1e9;
//
//  auto rot = getObject()->getRot();
//  distance = getObject()->getTransform2d().revertRotation(distance);
//
//  if (distance.x + distance.y == 0)
//  {
//    return;
//  }
//
//  double angleDiff = d->desiredAngle - rot;
//  
//  double desiredSpeed = 0;
//
///*  if (std::fabs(angleDiff) > 0.01)
//  {
//    for (Engine* engine : d->engines[eBackward])
//    {
//      engine->setConstantThrustP(0.0);
//    }
//    for (Engine* engine : d->engines[eForward])
//    {
//      engine->setConstantThrustP(0.0);
//    }
//    for (Engine* engine : d->engines[eLeft])
//    {
//      engine->setConstantThrustP(0.0);
//    }
//    for (Engine* engine : d->engines[eRight])
//    {
//      engine->setConstantThrustP(0.0);
//    }
//    return;
//  }
//*/
//  /*
//  mika::fixAngle(angleDiff);
//  glm::f64vec2 decelRate;
//  if (std::abs(angleDiff) > M_PI/2)
//  {
//    decelRate.y = d->
//    yDecel = -d->reverseAccel;
//    yAccel = d->forwardAccel;
//  }
//  else
//  {
//    yDecel = d->forwardAccel;
//    yAccel = -d->reverseAccel;
//  }
//  if (angleDiff > 0)
//  {
//    xAccel = -d->leftAccel;
//    xDecel = d->rightAccel;
//  }
//  else
//  {
//    xDecel = -d->leftAccel;
//    xAccel = d->rightAccel;
//  }*/
//
//
//  auto mySpeed = getObject()->getTransform2d().revertRotation(speed);
//
//  glm::fvec2 desiredAccel;
//  
//  double projectedPoint = distance.y - mySpeed.y * seconds;
//  double projectedAccel = distance.y - (mySpeed.y + d->forwardAccel * seconds) * seconds;
//  double projectedDecel = distance.y - (mySpeed.y + d->reverseAccel * seconds) * seconds;
//
//  if (distance.y > 0)
//  {
//    desiredSpeed = std::sqrt(1.23 * distance.y * -d->reverseAccel);
//  }
//  else
//  {
//    desiredSpeed = -std::sqrt(1.23 * -distance.y * d->forwardAccel);
//  }
//  desiredAccel.y = (desiredSpeed - mySpeed.y) / seconds;
//
//  projectedPoint = distance.x - mySpeed.x * seconds;
//  projectedAccel = distance.x - (mySpeed.x + d->leftAccel * seconds) * seconds;
//  projectedDecel = distance.x - (mySpeed.x + d->rightAccel * seconds) * seconds;
//
//  if (distance.x > 0)
//  {
//    desiredSpeed = std::sqrt(1.23 * distance.x * -d->leftAccel);
//  }
//  else
//  {
//    desiredSpeed = -std::sqrt(1.23 * -distance.x * d->rightAccel);
//  }
//  desiredAccel.x = (desiredSpeed - mySpeed.x) / seconds;
//
//
//  glm::f64vec2 thrust;
//  if (desiredAccel.y > 0)
//  {
//    thrust.y = desiredAccel.y / d->forwardAccel;
//  }
//  else
//  {
//    thrust.y = desiredAccel.y / -d->reverseAccel;
//  }
//  if (desiredAccel.x > 0)
//  {
//    thrust.x = desiredAccel.x / d->rightAccel;
//  }
//  else
//  {
//    thrust.x = desiredAccel.x / -d->leftAccel;
//  }
//
//  if (std::abs(thrust.x) > std::abs(thrust.y))
//  {
//    thrust /= std::abs(thrust.x);
//  }
//  else
//  {
//    thrust /= std::abs(thrust.y);
//  }
//
//  if (thrust.y > 0)
//  {
//    for (Engine* engine : d->engines[eBackward])
//    {
//      engine->setConstantThrustP(0.0);
//    }
//    for (Engine* engine : d->engines[eForward])
//    {
//      engine->setConstantThrustP(thrust.y);
//    }
//  }
//  else
//  {
//    for (Engine* engine : d->engines[eBackward])
//    {
//      engine->setConstantThrustP(-thrust.y);
//    }
//    for (Engine* engine : d->engines[eForward])
//    {
//      engine->setConstantThrustP(0.0);
//    }
//  }
//
//  if (thrust.x > 0)
//  {
//    for (Engine* engine : d->engines[eLeft])
//    {
//      engine->setConstantThrustP(0.0);
//    }
//    for (Engine* engine : d->engines[eRight])
//    {
//      engine->setConstantThrustP(thrust.x);
//    }
//  }
//  else
//  {
//    for (Engine* engine : d->engines[eLeft])
//    {
//      engine->setConstantThrustP(-thrust.x);
//    }
//    for (Engine* engine : d->engines[eRight])
//    {
//      engine->setConstantThrustP(0.0);
//    }
//  }
//
//  static double time = 0;
//  time += seconds;
//
//  std::vector<double> values;
//  values.push_back(time);
//  values.push_back(distance.x / 1e6);
//  values.push_back(distance.y / 1e6);
//  values.push_back(thrust.x);
//  values.push_back(thrust.y);
//
//  std::cout << "Data: ";
//  for (auto value : values)
//  {
//    std::cout << value << ", ";
//  }
//  std::cout << "\n";
//}

void FlyByWireAi::updateLogic(glm::i64 p_nanos)
{
  calibrateEngines();

  if (d->target)
  {
    setDesiredState(d->target->getRot(),
                    0,
                    d->target->getPos() + d->positionOffset,
                    d->target->getSpeed());
  }

  handleRotation(p_nanos);
  handleLocation(p_nanos);

}

void FlyByWireAi::setMaxSpeed(glm::f32 p_percentage)
{
  d->maxSpeedPercentage = p_percentage;
}

void FlyByWireAi::setPositionOffset(glm::i64vec2 const& p_offset)
{
  d->positionOffset = p_offset;
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

void FlyByWireAi::setDesiredRotationSpeed(glm::f32 p_rotationSpeed)
{
  d->desiredRotationSpeed = p_rotationSpeed;
}

void FlyByWireAi::setDesiredState(glm::f32 p_direction, glm::f32 p_rotationSpeed, glm::i64vec2 const& p_position, glm::i64vec2 const& p_speed)
{
  setDesiredDirection(p_direction);
  setDesiredRotationSpeed(p_rotationSpeed);
  setDesiredLocation(p_position);
  setDesiredSpeedAtLocation(p_speed);
}


