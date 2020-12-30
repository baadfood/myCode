#include "Component.h"

#include "Asset.h"
#include "Physics/Fixture.h"
#include <Physics/Shape.h>
#include <AABB.h>
#include <Object.h>
#include "Physics/MassData.h"

struct Component::Private
{
  AABB aabb;
  std::vector<Asset *> assets;
  std::vector<Fixture *> fixtures;
  std::vector<Contact *> contacts;
  Transform2d transform;
  Transform2d worldTransform;
  Transform2D graphicsTransform;
  glm::mat4 model;
  glm::u64vec2 halfSize;
  Object * object;
  glm::f32vec3 selfIlluminationColor = glm::f32vec3(0.0,0.0,0.0);
  bool posIsWorldPos = false;

  MassData massData;
};

Component::Component():
d(new Private)
{
}

Component::~Component()
{
}

void Component::setWorldPos(glm::i64vec2 p_pos)
{
  d->posIsWorldPos = true;
  d->transform.pos = p_pos;
}

Object* Component::getObject()
{
  return d->object;
}

void Component::setObject(Object* p_object)
{
  d->object = p_object;
}

void Component::setSelfIllumination(glm::f32vec3 p_color)
{
  d->selfIlluminationColor = p_color;
}

glm::f32vec3 Component::getSelfIllumination() const
{
  return d->selfIlluminationColor;
}

void Component::addContact(Contact * p_contact)
{
  d->contacts.push_back(p_contact);
}
void Component::clearContacts()
{
  d->contacts.clear();
}

std::vector<Contact *> & Component::getContacts()
{
  return d->contacts;
}

const glm::mat4& Component::getModel() const
{
  return d->model;
}

void Component::advance(glm::u64 p_nanos, Object * p_object)
{
}

void Component::updatePosition(Object* p_object)
{
  if(d->posIsWorldPos)
  {
    d->worldTransform.pos = d->transform.pos;
  }
  else
  {
    d->worldTransform = d->transform * p_object->getTransform2d();
  }
}

AABB& Component::getAabb()
{
  return d->aabb;
}

void Component::computeAabb()
{
  d->aabb.reset();
  for(Fixture * fix : d->fixtures)
  {
    fix->shape->computeAabb(fix->shape->getAabb(), d->worldTransform);
    d->aabb += fix->shape->getAabb();
  }
}

void Component::updateLogic(glm::u64 p_nanos)
{

}

void Component::calculateMassData(MassData& p_massData)
{
  d->massData.mass = 0;
  d->massData.inertia = 0;
  d->massData.center = glm::i64vec2(0,0);
  
  for(Fixture * fix : d->fixtures)
  {
    if(fix->density == 0)
    {
      continue;
    }
    
    MassData mass;
    fix->shape->calculateMassData(mass, fix->density, d->transform);
    
    d->massData.mass += mass.mass;
    d->massData.center += mass.center * mass.mass;
    d->massData.inertia += mass.inertia;
  }

  if (d->massData.mass > 0.0f)
  {
    d->massData.center /= d->massData.mass;
  }

  p_massData = d->massData;
}

const MassData& Component::getMassdata() const
{
  return d->massData;
}

void Component::updateTransform(const glm::i64vec2& p_origin, glm::i64 p_worldPerPixel, glm::vec2 p_scale)
{
  d->graphicsTransform.getPos() = glm::vec2(double(d->worldTransform.pos.x - p_origin.x)/ p_worldPerPixel, double(d->worldTransform.pos.y - p_origin.y)/ p_worldPerPixel);
  d->graphicsTransform.getRot() = std::atan2(d->worldTransform.rot.y, d->worldTransform.rot.x);
  d->graphicsTransform.getScale() = p_scale;
  d->model = d->graphicsTransform.getModel();
}

const AABB& Component::getAabb() const
{
  return d->aabb;
}

void Component::addAsset(Asset * p_asset)
{
  d->assets.push_back(p_asset);
}

void Component::addFixture(Fixture * p_fix)
{
  d->fixtures.push_back(p_fix);
}

const std::vector<Asset * >& Component::getAssets() const
{
  return d->assets;
}

const std::vector<Fixture * >& Component::getFixtures() const
{
  return d->fixtures;
}

const Transform2d& Component::getTransform2d() const
{
  return d->transform;
}

const Transform2d& Component::getWorldTransform() const
{
  return d->worldTransform;
}

void Component::setAngle(glm::float32 p_angle)
{
  d->transform.setRotation(p_angle);
}

void Component::setPosition(glm::i64vec2 p_pos)
{
  d->transform.pos = p_pos;
}

glm::i64vec2 const & Component::getPosition() const
{
  return d->transform.pos;
}

glm::i64vec2 & Component::getPosition()
{
  return d->transform.pos;
}