#pragma once

#include "ShapeCollisionHandler.h"
#include "Physics/Collision/Collision.h"
#include "Physics/Contact.h"
#include "Physics/Shape.h"

#include <vector>

class Object;

class ShapeCollisionDetector
{
public:
  ShapeCollisionDetector();
  ~ShapeCollisionDetector();

  bool getCollisions(Object * p_object1, Object * p_object2, std::vector<Contact*> & p_results);

  void registerCollisionHandler(int p_type1, int p_type2, Collision * p_handler);

private:
  Collision* m_handlers[Shape::eTypeCount][Shape::eTypeCount];
  void calculateImpulses(Contact* newContact);
};

