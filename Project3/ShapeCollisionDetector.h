#pragma once

#include "ShapeCollisionHandler.h"
#include "Physics/Collision/Collision.h"
#include "Physics/Contact.h"

#include <vector>

class Object;

class ShapeCollisionDetector
{
public:
  ShapeCollisionDetector();
  ~ShapeCollisionDetector();

  bool getCollisions(Object const * p_object1, Object const * p_object2, std::vector<Contact*> & p_results);

  void registerCollisionHandler(int p_type1, int p_type2, Collision * p_handler);

private:
  std::vector<std::vector<Collision*>> m_handlers;
};

