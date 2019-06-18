#include "Object.h"

#include "CameraWorldBased.h"
#include "SpatialTree.h"

#include "utils.h"
#include "Physics/Shape.h"
#include "Physics/Contact.h"
#include "Physics/Manifold.h"

#include "Physics/CollisionIsland.h"
#include "Components/Component.h"
#include "AI/Ai.h"

#include <iostream>

#include <cassert>

Object::Object(std::string const & p_name, glm::i64vec2 p_pos, glm::u64vec2 p_halfSize, glm::i32vec2 p_speed, glm::float32 p_rot, glm::float32 p_rotSpeed, glm::i64vec2 p_origin) :
m_pos(p_pos),
m_ai(nullptr),
m_halfSize(p_halfSize),
m_speed(p_speed),
m_rot(p_rot),
m_rotSpeed(p_rotSpeed),
m_typeId(0),
m_treeNode(nullptr),
m_name(p_name),
m_treeNodeIndex(0),
m_rotAccel(0),
m_accel(0,0),
m_centerOfMass(0,0),
m_positionCorrectionPressure(0)
{
}

Object::~Object()
{
}
/*
void Object::getConnectedObjects(std::vector<Object*> & p_connectedObjects)
{
  if(m_collisionIsland == nullptr
  || m_collisionIsland.isNull())
  {
    m_collisionIsland = CollisionIsland::newCollisionIsland();
  }

  for (Contact * contact : m_contacts)
  {
    Object * other = contact->fixtures[1].object != this ? contact->fixtures[1].object : contact->fixtures[0].object;
    if (other->m_collisionIsland == nullptr)
    {
      other->m_collisionIsland = m_collisionIsland;
//      other->getConnectedObjects(p_connectedObjects);
    }
    else if (other->m_collisionIsland != m_collisionIsland)
    {
      other->m_collisionIsland.redirect(m_collisionIsland);
    }
  }
}
*/
void Object::updateIsland()
{
  if (m_contacts.empty())
  {
    return;
  }

  if (m_collisionIsland == nullptr)
  {
    bool set = false;
    for (Contact * contact : m_contacts)
    {
      if(contact->physical)
      {
        Object * other = contact->fixtures[1]->object != this ? contact->fixtures[1]->object : contact->fixtures[0]->object;
        if (other->m_collisionIsland != nullptr)
        {
          set = true;
          m_collisionIsland = other->m_collisionIsland;
          break;
        }
      }
    }

    if (!set)
    {
      m_collisionIsland = std::shared_ptr<CollisionIsland>(CollisionIsland::newCollisionIsland());
//      std::cout << this << " New collision island " << m_collisionIsland.get() << " : " << std::endl;
      for (Contact * contact : m_contacts)
      {
        if(contact->physical)
        {
          Object * other = contact->fixtures[1]->object != this ? contact->fixtures[1]->object : contact->fixtures[0]->object;
          other->m_collisionIsland = m_collisionIsland;
          m_collisionIsland->objects.push_back(other);
        }
      }
    }
    m_collisionIsland->objects.push_back(this);
  }

  for (Contact * contact : m_contacts)
  {
    Object * other = nullptr;
    if(contact->fixtures[0]->object == this)
    {
      contact->components[1]->addContact(contact);
      other = contact->fixtures[1]->object;
    }
    else
    {
      contact->components[0]->addContact(contact);
      other = contact->fixtures[0]->object;
    }
    if(contact->physical)
    {
      m_collisionIsland->objects.reserve(m_collisionIsland->objects.size() + m_contacts.size());
      if (other->m_collisionIsland == nullptr)
      {
        other->m_collisionIsland = m_collisionIsland;
  //      std::cout << this << " Single add " << m_collisionIsland.get() << " : " << other << std::endl;
        m_collisionIsland->objects.push_back(other);
  //      assert(mika::checkDuplicates(m_collisionIsland.get()->objects));
      }
      else if (other->m_collisionIsland != m_collisionIsland)
      {
  //      std::cout << this << " Merge add " << m_collisionIsland.get() << " : " << other->m_collisionIsland.get() << " : " << other << std::endl;
        m_collisionIsland->objects.insert(m_collisionIsland->objects.end(), other->m_collisionIsland->objects.begin(), other->m_collisionIsland->objects.end());
        for (auto object : other->m_collisionIsland->objects)
        {
          object->m_collisionIsland = m_collisionIsland;
        }
  //      assert(mika::checkDuplicates(m_collisionIsland.get()->objects));
  //      other->m_collisionIsland.redirect(m_collisionIsland); This didn't work
  /*      for (auto object : m_collisionIsland.get()->objects)
        {
          if (object->m_collisionIsland.get() != m_collisionIsland.get())
          {
            std::cout << object << " Wrong collision island " << object->m_collisionIsland.get() << std::endl;
          }
        }
  */
  //      other->updateIsland();
      }
    }
  }
}


void Object::addContact(Contact * p_manifold)
{
  m_contacts.push_back(p_manifold);
/*
  Object * other = p_manifold->fixtures[1].object;
  if (other == this)
  {
    other = p_manifold->fixtures[0].object;
  }
*/
}

CollisionIsland * Object::getCollisionIsland()
{
  return m_collisionIsland.get();
}


std::vector<Contact*> const & Object::getContacts()
{
  return m_contacts;
}

void Object::clearContacts()
{
  for(Component * component : m_components)
  {
    component->clearContacts();
  }
  for(Contact * contact : m_contacts)
  {
    Contact::freeContact(contact);
  }
  m_contacts.clear();
  m_collisionIsland.reset();
}

Transform2d const & Object::getTransform2d() const
{
  return m_physicsTransform;
}

void Object::setName(std::string const & p_name)
{
  m_name = p_name;
}

std::string const & Object::getName() const
{
  return m_name;
}

SpatialTree * Object::getTreeNode()
{
  return m_treeNode;
}

bool Object::isInCorrectQuadtreeNode() const
{
  return m_inCorrectQuadtreeNode;
}

void Object::setTreeNode(SpatialTree * p_node, size_t p_index)
{
  m_treeNode = p_node;
  m_treeNodeIndex = p_index;
  assert(p_node != nullptr);
}

void Object::updateTree()
{
  if(m_treeNode)
  {
    m_treeNode->updateObjectPos(this, m_treeNodeIndex);
  }
  else
  {
    assert(false);
    //ERROR;
  }
}

void Object::moveBy(glm::i64vec2 const & p_pos)
{
  m_pos += p_pos;
/*  if (m_pos.x < -922337177244012441
  ||  m_pos.y < -922337177244012441)
  {
    std::cout << "I'm far off\n";
  }*/
}

void Object::moveTo(glm::i64vec2 const & p_pos)
{
  m_pos = p_pos;
}

glm::int64 Object::getXPos() const
{
  return m_pos.x;
}

glm::int64 Object::getYPos() const
{
  return m_pos.y;
}

glm::i64vec2 const & Object::getPos() const
{
  return m_pos;
}

void Object::setHalfSize(glm::u64vec2 p_size)
{
  m_halfSize = p_size;
}

glm::u64vec2 const & Object::getHalfSize() const
{
  return m_halfSize;
}

void Object::setRot(glm::float32 p_rot)
{
  m_rot = p_rot;
}

glm::float32 Object::getRot() const
{
  return m_rot;
}

glm::i64vec2 Object::getOffset(glm::i64vec2 const & p_origin) const
{
  return m_pos - p_origin;
}

void Object::addAccel(glm::i64vec2 p_accel)
{
  m_accel += p_accel;
}

void Object::addRotAccel(glm::float32 p_rotAccel)
{
  m_rotAccel += p_rotAccel;
}

const glm::float32& Object::getRotSpeed() const
{
  return m_rotSpeed;
}

const glm::i64vec2& Object::getSpeed() const
{
  return m_speed;
}

void Object::setXPos(glm::int64 p_x)
{
  m_pos.x = p_x;
}
void Object::setYPos(glm::int64 p_y)
{
  m_pos.y = p_y;
}

void Object::updateTransform(glm::i64vec2 const & p_origin, glm::i64 p_worldPerPixel)
{
  m_transform.getPos() = glm::vec2(double(m_pos.x - p_origin.x)/ p_worldPerPixel, double(m_pos.y - p_origin.y)/ p_worldPerPixel);
  m_transform.getRot() = m_rot;
  m_transform.getScale() = glm::vec2(double(m_halfSize.x)/double(p_worldPerPixel), double(m_halfSize.y)/double(p_worldPerPixel));
  m_model = m_transform.getModel();
  for(Component * component : m_components)
  {
    component->updateTransform(p_origin, p_worldPerPixel, m_transform.getScale());
  }
}

glm::mat4 const & Object::getTransform() const
{
  return m_model;
}

void Object::updateAabb()
{
  if (m_components.empty() == false)
  {
    m_aabb.reset();
    if (m_components.empty() == false)
    {
      m_aabb.reset();
      for(Component * component : m_components)
      {
        component->computeAabb();
        m_aabb += component->getAabb();
      }
    }
  }
  else
  {
    m_aabb.m_pos = m_pos;
    m_aabb.m_halfSize = m_halfSize;
  }
}

AABB const & Object::getAABB() const
{
  return m_aabb;
}

bool Object::handleGotFocus(SDL_Event const * p_event)
{
  // By default, try to handle incoming event to see if we want focus.
  return handleInput(p_event);
}

void Object::updateLogic(glm::u64 p_nanos)
{
  for(Component * component : m_components)
  {
    component->updateLogic(p_nanos);
  }
  if(m_ai)
  {
    m_ai->updateLogic(p_nanos);
  }
}

glm::f64 & Object::positionCorrectionPressure()
{
  return m_positionCorrectionPressure;
}


void Object::handleLostFocus(SDL_Event const * p_event)
{

}

void Object::advance(glm::u64 p_nanos)
{
  for(Component * component : m_components)
  {
    component->advance(p_nanos, this);
  }
  double seconds = double(p_nanos) / 1e9;
  m_positionCorrectionPressure /= pow(seconds/8 + 1, 2);
  updateMass();

  m_rot += m_rotSpeed * seconds;
  m_rot += m_rotAccel * pow(seconds,2) / 2;
  while(m_rot > mika::pi)
  {
    m_rot -= 2*mika::pi;
  }
  while(m_rot < -mika::pi)
  {
    m_rot += 2*mika::pi;
  }
  
  m_rotSpeed += m_rotAccel * seconds;

  m_accel.x /= 2;
  m_accel.y /= 2;
  
  m_pos.x += m_speed.x * seconds;
  m_pos.y += m_speed.y * seconds;
  m_pos.x += m_accel.x * pow(seconds,2) / 2;
  m_pos.y += m_accel.y * pow(seconds,2) / 2;
  m_speed.x += m_accel.x * seconds;
  m_speed.y += m_accel.y * seconds;

  m_physicsTransform.pos = m_pos;
  m_physicsTransform.rot = glm::fvec2(cos(m_rot), sin(m_rot));

  if (m_components.empty() == false)
  {
    m_aabb.reset();
    for(Component * component : m_components)
    {
      component->updatePosition(this);
      component->computeAabb();
      m_aabb += component->getAabb();
    }
  }
}

std::vector<Component*> const & Object::getComponents() const
{
  return m_components;
}

void Object::addComponent(Component * p_component)
{
  m_massDirty = true;
  m_components.push_back(p_component);
  p_component->setObject(this);
}

void Object::removeComponent(Component * p_component)
{
  m_massDirty = true;
  mika::removeOne(m_components, p_component);
  p_component->setObject(nullptr);
}

glm::u32 Object::getTypeId() const
{
  return m_typeId;
}

bool Object::handleInput(SDL_Event const * p_event)
{
  for (auto iter = m_inputHandlers.begin();
  iter != m_inputHandlers.end();
    iter++)
  {
    (*iter)->bindObject(this);
    if ((*iter)->handleInput(p_event))
    {
      return true;
    }
  }
  return false;
}

void Object::bindObject(Object * p_object)
{
}

void Object::addInputHandler(std::shared_ptr<UserInputHandler> p_handler)
{
  m_inputHandlers.push_back(p_handler);
}

void Object::removeInputHandler(std::shared_ptr<UserInputHandler> p_handler)
{
  mika::removeOne(m_inputHandlers, p_handler);
}

void Object::setAccel(glm::i64vec2 p_accel)
{
  m_accel = p_accel;
}

void Object::setRotAccel(glm::float32 p_rotAccel)
{
  m_rotAccel = p_rotAccel;
}

void Object::setRotSpeed(glm::float32 p_rotSpeed)
{
  m_rotSpeed = p_rotSpeed;
}

void Object::setSpeed(glm::i64vec2 p_speed)
{
  m_speed = p_speed;
}

glm::f64 Object::getInertia() const
{
  return m_inertia;
}

glm::f64 Object::getInvInertia() const
{
  return m_invInertia;
}

glm::f64 Object::getInvMass() const
{
  return m_invMass;
}

glm::f64 Object::getMass() const
{
  return m_mass;
}

void Object::setMassDirty()
{
	m_massDirty = true;
}

void Object::updateMass()
{
	if (m_massDirty == false)
	{
		return;
	}
  m_mass = 0.0f;
  m_invMass = 0.0f;
  m_inertia = 0.0f;
  m_invInertia = 0.0f;
  
  glm::i64vec2 oldCenter = m_centerOfMass;
  
  // Accumulate mass over all fixtures.
  m_centerOfMass.x = 0;
  m_centerOfMass.y = 0;
  
  for(Component * component: m_components)
  {
    MassData mass;
    component->calculateMassData(mass);
    
    m_mass += mass.mass;
    m_centerOfMass += mass.center * mass.mass;
    m_inertia += mass.inertia;
  }

  // Compute center of mass.
  if (m_mass > 0.0f)
  {
    m_invMass = 1.0f / m_mass;
    m_centerOfMass *= m_invMass;
  }
  else
  {
    // Force all dynamic bodies to have a positive mass.
    m_mass = 1.0f;
    m_invMass = 1.0f;
  }

  if (m_inertia > 0.0f)
  {
    // Center the inertia about the center of mass.
    m_inertia -= m_mass * glm::dot(glm::f64vec2(m_centerOfMass), glm::f64vec2(m_centerOfMass));
    m_invInertia = 1.0f / m_inertia;
  }
  else
  {
    m_inertia = 0.0f;
    m_invInertia = 0.0f;
  }
  
  glm::i64vec2 centerOfMassCorrection = m_centerOfMass - oldCenter;
  centerOfMassCorrection /= 2;
  
  for(Component * component: m_components)
  {
    component->getPosition() -= centerOfMassCorrection;
  }

  // Update center of mass velocity.
//  m_speed += mika::crossS(m_rotSpeed, m_centerOfMass - oldCenter);
}

void Object::applyImpulse(glm::f64vec2 p_impulse, glm::f64vec2 p_contactVector)
{
  m_speed += glm::i64vec2(m_invMass * p_impulse);
  m_rotSpeed += m_invInertia * mika::cross(p_contactVector, p_impulse);
}

void Object::storeImpulse(glm::f64vec2 p_impulse, glm::f64vec2 p_contactVector)
{
  m_stored_speed += glm::i64vec2(m_invMass * p_impulse);
  m_stored_rotSpeed += m_invInertia * mika::cross(p_contactVector, p_impulse);
}

void Object::applyStoredImpulses()
{
  m_speed += m_stored_speed;
  m_rotSpeed += m_stored_rotSpeed;
  
  m_stored_speed = glm::i64vec2(0,0);
  m_stored_rotSpeed = 0;
}

void Object::calculateImpluseEffect(glm::f64vec2 p_impulse, glm::f64vec2 p_contactVector, glm::i64vec2& p_accel, float& p_rotSpeed)
{
  p_accel = glm::i64vec2(m_invMass * p_impulse);
  p_rotSpeed += m_invInertia * mika::cross(p_contactVector, p_impulse);
}

void Object::setAi(Ai* p_ai)
{
  m_ai = p_ai;
}

void Object::print()
{
  std::stringstream sstream;
  
  sstream << "Object " << m_name << std::endl;
  sstream << "Rot " << m_rot << std::endl;
  sstream << "RotSpeed " << m_rotSpeed << std::endl;
  
  std::cout << sstream.str();
}

