#include "Object.h"

#include "CameraWorldBased.h"
#include "SpatialTree.h"

#include "utils.h"
#include "Physics/Shape.h"
#include "Physics/Contact.h"

#include <cassert>

Object::Object(std::string const & p_name, glm::i64vec2 p_pos, glm::u64vec2 p_halfSize, glm::i32vec2 p_speed, glm::float32 p_rot, glm::float32 p_rotSpeed, glm::i64vec2 p_origin) :
m_pos(p_pos),
m_halfSize(p_halfSize),
m_speed(p_speed),
m_rot(p_rot),
m_rotSpeed(p_rotSpeed),
m_typeId(0),
m_treeNode(nullptr),
m_name(p_name),
m_treeNodeIndex(0),
m_rotAccel(0),
m_accel(0,0)
{
}

Object::~Object()
{
}

void Object::getConnectedObjects(std::vector<Object*> & p_connectedObjects)
{
  p_connectedObjects.reserve(p_connectedObjects.size() + m_contacts.size());
  for (Contact * contact : m_contacts)
  {
    auto iter = p_connectedObjects.begin();
    for(;
        iter != p_connectedObjects.end();
        iter++)
    {
      if(*iter == contact->fixtures[1].object)
      {
        break;
      }
    }
    
    if(iter == p_connectedObjects.end())
    {
      p_connectedObjects.push_back(contact->fixtures[1].object);
      contact->fixtures[1].object->getConnectedObjects(p_connectedObjects);
    }
  }
}


void Object::addContact(Contact * p_manifold)
{
  m_contacts.push_back(p_manifold);
}

std::vector<Contact*> const & Object::getContacts()
{
  return m_contacts;
}

void Object::clearContacts()
{
  m_contacts.clear();
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

void Object::setAsset(std::shared_ptr<Asset> p_Asset)
{
  m_asset = p_Asset;
}
std::shared_ptr<Asset> Object::getAsset()
{
  return m_asset;
}

void Object::moveBy(glm::i64vec2 const & p_pos)
{
  m_pos += p_pos;
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
}

glm::mat4 const & Object::getTransform() const
{
  return m_model;
}

void Object::updateAabb()
{
  m_aabb.setCenter(m_pos);
  m_aabb.setSize(m_halfSize);
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

void Object::updateLogic()
{

}

void Object::handleLostFocus(SDL_Event const * p_event)
{

}

void Object::advance(glm::u64 p_nanos)
{
  double seconds = double(p_nanos) / 1e9;
  
  m_rot += m_rotSpeed * seconds;
  m_rot += m_rotAccel * pow(seconds,2) / 2;
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
  m_physicsTransform.rot = glm::fvec2(sin(m_rot), cos(m_rot));

  for(auto iter = m_fixtures.begin();
      iter != m_fixtures.end();
      iter++)
  {
    Fixture * fixture = (*iter);
    fixture->shape->computeAabb(fixture->shape->getAabb(), m_physicsTransform);
  }
}

std::vector<Fixture*> const & Object::getFixtures() const
{
  return m_fixtures;
}

void Object::addFixture(Fixture * p_fixture)
{
  m_fixtures.push_back(p_fixture);
}

void Object::removeFixture(Fixture * p_fixture)
{
  mika::removeOne(m_fixtures, p_fixture);
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

void Object::updateMass()
{
  m_mass = 0.0f;
  m_invMass = 0.0f;
  m_inertia = 0.0f;
  m_invInertia = 0.0f;
  
  glm::i64vec2 oldCenter = m_centerOfMass;
  
  // Accumulate mass over all fixtures.
  m_centerOfMass.x = 0;
  m_centerOfMass.y = 0;
  
  for(Fixture * fix : m_fixtures)
  {
    if(fix->density == 0)
    {
      continue;
    }
    
    MassData mass;
    fix->shape->calculateMassData(mass, fix->density);
    
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

  // Update center of mass velocity.
  m_speed += mika::crossS(m_rotSpeed, m_centerOfMass - oldCenter);
}

void Object::applyImpulse(glm::f64vec2 p_impulse, glm::f64vec2 p_contactVector)
{
  m_speed += glm::i64vec2(m_invMass * p_impulse);
  m_rotSpeed += m_invInertia * mika::cross(p_contactVector, p_impulse);
}

