#include "Sensor.h"
#include <Physics/Contact.h>

struct Sensor::Private
{
  std::vector<Fixture *> sensors;
  std::vector<Contact *> sensorContacts;
};

Sensor::Sensor():
d(new Private)
{
}

Sensor::~Sensor()
{

}

void Sensor::addSensorFixture(Fixture * p_sensorFixture)
{
  d->sensors.push_back(p_sensorFixture);
  addFixture(p_sensorFixture);
}

void Sensor::updateLogic(glm::u64 p_nanos)
{
  std::vector<Contact*> & contacts = getContacts();
  d->sensorContacts.clear();
  for(Contact * contact : contacts)
  {
    for(Fixture * sensor : d->sensors)
    {
      if(contact->fixtures[0] == sensor
      || contact->fixtures[1] == sensor)
      {
        d->sensorContacts.push_back(contact);
        break;
      }
    }
  }
}

const std::vector< Contact* >& Sensor::getSensorContacts()
{
  return d->sensorContacts;
}


