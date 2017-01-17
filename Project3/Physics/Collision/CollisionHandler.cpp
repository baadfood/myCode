#include "Physics/Collision/CollisionHandler.h"

namespace
{
  static Collision* g_collisionHandlers[Shape::eTypeCount][Shape::eTypeCount];
}


Collision* CollisionHandler::get(Shape::EType p_type1, Shape::EType p_type2)
{
  return g_collisionHandlers[p_type1][p_type2];
}

void CollisionHandler::set(Shape::EType p_type1, Shape::EType p_type2, Collision* p_handler)
{
  g_collisionHandlers[p_type1][p_type2] = p_handler;
}
