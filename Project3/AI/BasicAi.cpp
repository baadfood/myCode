#include "BasicAi.h"
#include <Object.h>

#include <Components/Engine.h>
#include <Components/Sensor.h>
#include <Physics/Contact.h>

#include "utils.h"

struct BasicAi::Private
{
  Object * target = nullptr;
  std::vector<Sensor*> sensors;
  std::vector<Engine*> engines[eEngineDirectionCount];
  Component * reticule;
  float prevDiff;
  
  double forwardAccel = 0;
  float clockWiseRate = 0;
  float counterWiseRate = 0;
};

BasicAi::BasicAi():
d(new Private())
{
  
}

BasicAi::~BasicAi()
{
  
}

void BasicAi::setTargettingReticule(Component* p_reticule)
{
  d->reticule = p_reticule;
}

void BasicAi::addSensor(Sensor* p_sensor)
{
  d->sensors.push_back(p_sensor);
}

void BasicAi::addEngine(Engine * p_engine, EEngineDirection p_direction)
{
  d->engines[p_direction].push_back(p_engine);
}

bool findObject(Contact const * p_contact, Object * p_object)
{
  if(p_contact->fixtures[0]->object == p_object
    || p_contact->fixtures[1]->object == p_object)
  {
    return true;
  }
  return false;
}

void BasicAi::updateLogic(glm::i64 p_nanos)
{
  Ai::updateLogic(p_nanos);

  Object * myObject = getObject();
  Object * newTarget = d->target;
  bool found = false;
  for(Sensor * sensor : d->sensors)
  {
    std::vector<Contact*> const & contacts = sensor->getSensorContacts();
    if(contacts.size() != 0)
    {
      if(d->target == nullptr)
      {
        d->prevDiff = 0;
        d->target = contacts[0]->getNotMe(myObject);
        found = true;
        break;
      }
      else
      {
        if(std::find_if(contacts.begin(), contacts.end(), std::bind(findObject, std::placeholders::_1, d->target)) != contacts.end())
        {
          found = true;
          break;
        }
        else
        {
          // old target gone, find new.
          if(newTarget == nullptr)
          {
            newTarget = contacts[0]->getNotMe(myObject);
          }
        }
      }
    }
  }
  if(found == false)
  {
    // Old target lost, need new target.
    d->target = newTarget;
    d->prevDiff = 0;
  }
  
  if(d->target)
  {
    for(Sensor * sensor : d->sensors)
    {
      sensor->setEnabled(false);
    }
    
    glm::f64vec2 distanceToTarget = d->target->getPos() - myObject->getPos();
    glm::f64vec2 relativeSpeed = d->target->getSpeed() - myObject->getSpeed();
    
    glm::float32 myAngle = myObject->getRot();
//    glm::float32 targetAtAngle = atan2(distanceToTarget.y, distanceToTarget.x);
    glm::f64vec2 unitVectorToTarget = glm::normalize(distanceToTarget);
    glm::f64vec2 unitVectorRelativeSpeed = glm::normalize(relativeSpeed);
    
    
    glm::f64vec2 aimVector;
    
    d->forwardAccel = 0;
    for(Engine * engine : d->engines[eForward])
    {
      glm::i64vec2 linear;
      float angular = 0;
      engine->getEffectOnObject(1e9, 1.0, linear, angular);
      d->forwardAccel += myObject->getTransform2d().revertRotation(linear).y;
    }
    
    if(relativeSpeed.x == 0 && relativeSpeed.y == 0)
    {
      aimVector = unitVectorToTarget;
    }
    //      else
    //      {
    //        aimVector = glm::normalize(unitVectorToTarget + (-unitVectorRelativeSpeed * 0.1));
    //      }
    else
    {
      Transform2d rotation;
      rotation.rot.x = -unitVectorToTarget.x;
      rotation.rot.y = unitVectorToTarget.y;
      
//      double scalarDistanceToTarget = sqrt(distanceToTarget.x * distanceToTarget.x + distanceToTarget.y + distanceToTarget.y);
      
      glm::f64vec2 rotatedRelativeSpeed = rotation.applyRotation(relativeSpeed);
      
//      double scalarRelativeSpeed = rotatedRelativeSpeed.x - abs(rotatedRelativeSpeed.y);
      
//      double timeToTarget = (-scalarRelativeSpeed + sqrt(scalarRelativeSpeed*scalarRelativeSpeed + 2 * d->forwardAccel * scalarDistanceToTarget)) / d->forwardAccel;
//      glm::f64vec2 targetProjectedPos = glm::f64vec2(d->target->getPos()) + glm::f64vec2(d->target->getSpeed()) * timeToTarget;
//      glm::f64vec2 myProjectedPos = glm::f64vec2(myObject->getPos()) + glm::f64vec2(myObject->getSpeed()) * timeToTarget; 
//      aimVector = glm::normalize(targetProjectedPos - myProjectedPos);
//      std::cout << "Time To Target " << timeToTarget << std::endl;
//      d->reticule->setWorldPos(glm::i64vec2(myProjectedPos));
//      std::cout << "Relative speed coeff " << std::min(0.99, std::abs(rotatedRelativeSpeed.y /(d->forwardAccel*2.0))) << std::endl;
//      std::cout << "Relative y speed " << rotatedRelativeSpeed.y << std::endl;
      aimVector = glm::normalize(unitVectorToTarget + (unitVectorRelativeSpeed * std::min(4.0, std::abs(rotatedRelativeSpeed.y / (d->forwardAccel*0.5)))));
    }
    
	glm::float32 angleToAimAt = atan2(-aimVector.x, aimVector.y);
	glm::float32 angleDiff = angleToAimAt - myAngle;
	//    glm::float32 timeToHeading = std::abs(angleDiff / myObject->getRotSpeed());
	mika::fixAngle(angleDiff);
	
	for(Sensor * sensor : d->sensors)
  {
    sensor->setAngle(angleDiff);
  }
    
    
    // TODO This logic might be bad, I have no idea.
    if(std::abs(angleDiff) < mika::pi/16)
    {
      for(Engine * forwardEngine : d->engines[eForward])
      {
        forwardEngine->setConstantThrustP(1.0);
      }
    }
    else
    {
      for(Engine * forwardEngine : d->engines[eForward])
      {
        forwardEngine->setConstantThrustP(0.1);
      }
    }
    
    bool turnClockwise = false;
    float thrust = 1.0;
    
    // TODO move engine calibration to a different place and only do it when engine/hull properties have changed.
    d->clockWiseRate = 0;
    for(Engine * engine : d->engines[eClockwiseTurn])
    {
      glm::i64vec2 throwAway;
      float temp = 0;
      engine->getEffectOnObject(1e9, 1.0, throwAway, temp);
      d->clockWiseRate += temp;
    }
    d->counterWiseRate = 0;
    for(Engine * engine : d->engines[eCounterCTurn])
    {
      glm::i64vec2 throwAway;
      float temp = 0;
      engine->getEffectOnObject(1e9, 1.0, throwAway, temp);
      d->counterWiseRate += temp;
    }
    
//    std::cout << "AngleDiff " << angleDiff << std::endl;
    
    double rotSpeed = myObject->getRotSpeed();
//    double rotSpeedSquared = rotSpeed * rotSpeed;
    
    if(angleDiff < 0)
    {
      float desiredSpeed = -std::sqrt(2 * d->clockWiseRate * angleDiff); // TODO get rid of square root
      double desiredAccel = (desiredSpeed - rotSpeed) / ((double(p_nanos) / 1e9));
      if(desiredAccel > 0)
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
      float desiredSpeed = std::sqrt(2 * d->counterWiseRate * angleDiff); // TODO get rid of square root
      double desiredAccel = (desiredSpeed - rotSpeed) / ((double(p_nanos) / 1e9));
      if(desiredAccel > 0)
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
    
    if(turnClockwise)
    {
      for(Engine * engine : d->engines[eCounterCTurn])
      {
        engine->setConstantThrustP(0.0);
      }
      for(Engine * engine : d->engines[eClockwiseTurn])
      {
        engine->setConstantThrustP(thrust);
      }
    }
    else
    {
      for(Engine * engine : d->engines[eCounterCTurn])
      {
        engine->setConstantThrustP(thrust);
      }
      for(Engine * engine : d->engines[eClockwiseTurn])
      {
        engine->setConstantThrustP(0.0);
      }
    }
  }
  else
  {
    for(Sensor * sensor : d->sensors)
    {
//		sensor->setAngle(0);
    }
  }
}


