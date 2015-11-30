#ifndef Object_h_
#define Object_h_

#include "Base.h"

#include "Asset.h"
#include "AABB.h"
#include "Transform2D.h"
#include "Physics\Transform.h"
#include "UserInputHandler.h"
#include <glm\glm.hpp>
#include <vector>
#include <memory>
#include "Spinlock.h"
#include "Physics\Fixture.h"
#include "Physics\Contact.h"

class Contact;
class SpatialTree;
static const glm::i64 OBJTOWORLD = 2147483648;
class CameraWorldBased;
class Fixture;

class Object : public Base, public UserInputHandler
{
public:
  Object(std::string const & p_name = "",
         glm::i64vec2 p_pos = glm::i64vec2(0, 0),
         glm::u64vec2 p_halfSize = glm::u64vec2(0, 0),
         glm::i32vec2 p_speed = glm::i32vec2(0, 0),
         glm::float32 p_rot = 0,
         glm::float32 p_rotSpeed = 0,
         glm::i64vec2 p_origin = glm::i64vec2(0, 0));

  virtual ~Object();

  virtual void getConnectedObjects(std::vector<Object*> p_connectedObjects);

  virtual void addContact(Contact * p_manifold);
  virtual std::vector<Contact*> const & getContacts();
  virtual void clearContacts();

  virtual void setName(std::string const & p_name);
  virtual std::string const & getName() const;

  virtual void setAsset(std::shared_ptr<Asset> p_Asset);
  virtual std::shared_ptr<Asset> getAsset();

  virtual AABB const & getAABB() const;

  virtual SpatialTree * getTreeNode();
  virtual void setTreeNode(SpatialTree * p_node, size_t p_index);
  virtual void updateTree();

  virtual void advance(glm::u64 p_nanos);

  virtual void moveBy(glm::i64vec2 const & p_pos);
  virtual void moveTo(glm::i64vec2 const & p_pos);
  virtual void setXPos(glm::int64 p_x);
  virtual void setYPos(glm::int64 p_y);

  virtual glm::int64 getXPos() const;
  virtual glm::int64 getYPos() const;
  virtual glm::i64vec2 const & getPos() const;

  virtual Transform2d const & getTransform2d() const;

  virtual void setHalfSize(glm::u64vec2 p_size);
  virtual glm::u64vec2 const & getHalfSize() const;

  virtual void setRot(glm::float32 p_rot);
  virtual glm::float32 getRot() const;

  virtual std::vector<Fixture*> const & getFixtures() const;
  virtual void addFixture(Fixture * p_fixture);
  virtual void removeFixture(Fixture * p_fixture);

  virtual void updateTransform(glm::i64vec2 const & p_origin = glm::i64vec2(0, 0), glm::i64 p_worldPerPixel = 1);

  virtual glm::u32 getTypeId() const;

//  virtual void setOrigin(CameraWorldBased * p_origin);
  virtual glm::i64vec2 getOffset(glm::i64vec2 const & p_origin) const;
  virtual glm::mat4 const & getTransform() const;

  virtual bool handleGotFocus(SDL_Event const * p_event);
  virtual void handleLostFocus(SDL_Event const * p_event);

  virtual bool handleInput(SDL_Event const * p_event);
  virtual void bindObject(Object * p_object);

  virtual void addInputHandler(std::shared_ptr<UserInputHandler> p_handler);
  virtual void removeInputHandler(std::shared_ptr<UserInputHandler> p_handler);
protected:

  virtual void updateAabb();

private:
  std::vector<std::shared_ptr<UserInputHandler>> m_inputHandlers;

  std::vector<Fixture*> m_fixtures;
  std::vector<Contact*> m_contacts;

  std::shared_ptr<Asset> m_asset;
  Spinlock m_lock;
  SpatialTree * m_treeNode;
  AABB m_aabb;
  size_t m_treeNodeIndex;

  std::string m_name;

  glm::mat4 m_model;
  glm::u32 m_typeId;

  glm::i64vec2 m_pos;
  glm::u64vec2 m_halfSize;
  glm::i64vec2 m_speed;
  glm::i64vec2 m_accel;

  glm::float32 m_rot;
  glm::float32 m_rotSpeed;
  glm::float32 m_rotAccel;

  Transform2D m_transform;
  Transform2d m_physicsTransform;
};

#endif
