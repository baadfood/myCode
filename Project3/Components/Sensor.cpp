#include "Sensor.h"
#include <Physics/Contact.h>
#include "SpatialTree.h"
#include "Physics/Collision/CollisionHandler.h"
#include "Physics/Collision/Collision.h"

struct Sensor::Private
{
  std::vector<Fixture *> physical;
  std::vector<Fixture *> sensors;
  std::vector<Contact *> sensorContacts;
  AABB sensorAabb;
  bool enabled = true;
};

Sensor::Sensor():
d(new Private)
{
}

Sensor::~Sensor()
{

}

void Sensor::setEnabled(bool p_value)
{
  d->enabled = p_value;
}


void Sensor::computeAabb()
{
  AABB & aabb = getAabb();
  aabb.reset();
  d->sensorAabb.reset();
  for(Fixture * fix : d->sensors)
  {
    fix->shape->computeAabb(fix->shape->getAabb(), getWorldTransform());
    d->sensorAabb += fix->shape->getAabb();
  }
  for(Fixture * fix : d->physical)
  {
    fix->shape->computeAabb(fix->shape->getAabb(), getWorldTransform());
    aabb += fix->shape->getAabb();
  }
}

void Sensor::addFixture(Fixture* p_fix)
{
  d->physical.push_back(p_fix);
  Component::addFixture(p_fix);
}

void Sensor::addSensorFixture(Fixture * p_sensorFixture)
{
  d->sensors.push_back(p_sensorFixture);
  Component::addFixture(p_sensorFixture);
}

void Sensor::updateLogic(glm::u64 p_nanos)
{
  for(Contact * contact : d->sensorContacts)
  {
    Contact::freeContact(contact);
  }
  d->sensorContacts.clear();
  
  if(d->enabled == false)
  {
    return;
  }

  std::vector<Object*> objects;
  Object * myObject = getObject();
  myObject->getTreeNode()->getObjectsIntersected(d->sensorAabb, objects);
  
  for(Object * object : objects)
  {
    if(object == myObject)
    {
      continue;
    }
    std::vector<Component *> const & components = object->getComponents();
    for(Component * comp : components)
    {
      std::vector<Fixture * > const & fixtures = comp->getFixtures();
      for (Fixture * fix : fixtures)
      {
        for (Fixture * sensorFixture : d->sensors)
        {
          if(Fixture::fixturesInteract(*fix, *sensorFixture))
          {
            if(sensorFixture->shape->getAabb().intersects(fix->shape->getAabb()))
            {
              Transform2d trans1 = getTransform2d() * getObject()->getTransform2d();
              Transform2d trans2 = comp->getTransform2d() * object->getTransform2d();
              Contact *newContact = nullptr;
              if (CollisionHandler::get(sensorFixture->shape->getType(), fix->shape->getType())->Collide(newContact, *sensorFixture, trans1, *fix, trans2))
              {
                newContact->components[0] = this;
                newContact->components[1] = comp;
                newContact->physical = false;
                d->sensorContacts.push_back(newContact);
              }
            }
          }
        }
      }
    }
  }
}

const std::vector< Contact* >& Sensor::getSensorContacts()
{
  return d->sensorContacts;
}


