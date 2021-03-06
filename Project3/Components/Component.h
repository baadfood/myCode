#pragma once

#include <vector>
#include <memory>
#include "Physics/Transform.h"

struct AABB;
class Asset;
class Fixture;
struct MassData;
class Object;
class Contact;

class Component
{
public:
  Component();
  ~Component();
  
  void addContact(Contact * p_contact);
  void clearContacts();
  std::vector<Contact *> & getContacts();

  virtual void updateLogic(glm::u64 p_nanos);

  void setSelfIllumination(glm::f32vec3 p_color);
  glm::f32vec3 getSelfIllumination() const;

  virtual void advance(glm::u64 p_nanos, Object * p_object);
  virtual void computeAabb();
  AABB const & getAabb() const;
  AABB & getAabb();
  
  virtual void addFixture(Fixture * p_fix);
  void addAsset(Asset * p_asset);

  MassData const & getMassdata() const;
  
  void calculateMassData(MassData & p_massData);

  Transform2d const & getTransform2d() const;
  Transform2d const & getWorldTransform() const;
  void updateTransform(glm::i64vec2 const & p_origin = glm::i64vec2(0, 0), glm::i64 p_worldPerPixel = 1, glm::vec2 p_scale = glm::vec2(1,1));
  
  void setWorldPos(glm::i64vec2 p_pos);
  void setPosition(glm::i64vec2 p_pos);
  void setAngle(glm::float32 p_angle);
  
  glm::i64vec2 const & getPosition() const;
  glm::i64vec2 & getPosition();
  
  std::vector<Asset * > const & getAssets() const;
  std::vector<Fixture * > const & getFixtures() const;
  
  glm::mat4 const & getModel() const;
  
  void setObject(Object* p_object);
  Object * getObject();
  void updatePosition(Object* p_object);
  
private:
  struct Private;
  std::unique_ptr<Private> d;
};