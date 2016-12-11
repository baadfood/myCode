#include "BasicAi.h"
#include <Object.h>

struct BasicAi::Private
{
  std::vector<Sensor*> sensors;
};

BasicAi::BasicAi():
d(new Private())
{

}

BasicAi::~BasicAi()
{

}

void BasicAi::addSensor(Sensor* p_sensor)
{
  d->sensors.push_back(p_sensor);
}

void BasicAi::updateLogic(glm::i64 p_nanos)
{
  Ai::updateLogic(p_nanos);

  bool boost = false;
  for(Sensor * sensor : d->sensors)
  {
    if(sensor->getSensorContacts().size() != 0)
    {
      boost = true;
      break;
    }
  }
  if(boost)
  {
    float angle = getObject()->getRot();
    getObject()->setRotSpeed(getObject()->getRotSpeed() * 0.90);
    getObject()->addAccel(glm::i64vec2(OBJTOWORLD * -sin(angle), OBJTOWORLD * cos(angle)));
  }
}


