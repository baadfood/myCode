#pragma once

#include <vector>
#include "Physics/Collision/Collision.h"

class Shape;

class ShapeCollisionHandler
{
public:
  void handleCollision(Shape const * p_shape1, Shape const * p_shape2, std::vector<Collision *> const & p_results);
};