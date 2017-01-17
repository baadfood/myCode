#ifndef CollisionHandler_
#define CollisionHandler_

#include "Physics/Shape.h"

class Collision;

class CollisionHandler
{
public:
  static Collision * get(Shape::EType p_type1, Shape::EType p_type2);
  static void set(Shape::EType p_type1, Shape::EType p_type2, Collision * p_handler);
};

#endif