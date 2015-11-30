#ifndef CircleToCircleCollision_h_
#define CircleToCircleCollision_h_

#include "Collision.h"
#include "../Manifold.h"
#include "../Transform.h"
#include <glm\glm.hpp>
#include "../Shapes/CircleShape.h"

class CircleToCircleCollision: public Collision
{
public:
  
  virtual bool Collide(Manifold * p_manifold,
    CircleShape * p_circle1, Transform2d const & p_tf1,
    CircleShape * p_circle2, Transform2d const & p_tf2)
  {

    glm::i64vec2 circle1Pos = p_tf1.multiply(p_circle1->pos);
    glm::i64vec2 circle2Pos = p_tf2.multiply(p_circle2->pos);

    glm::i64vec2 delta = circle2Pos - circle1Pos;
    glm::u64 distance = glm::dot(delta, delta);
    glm::u64 limit = p_circle1->radius + p_circle2->radius;
    if (distance > limit)
    {
      return false;
    }

    p_manifold = Manifold::newManifold();

    p_manifold->pointCount = 0;
    p_manifold->type = Manifold::eCircles;
    p_manifold->localNormal;
    p_manifold->pointCount = 2;
    p_manifold->contactPoints[0].contactPoint = p_circle1->pos;
    p_manifold->contactPoints[1].contactPoint = p_circle2->pos;
    return true;
  }

/*
  b2Vec2 pA = b2Mul(xfA, p_circle1->m_p);
  b2Vec2 pB = b2Mul(xfB, circleB->m_p);

  b2Vec2 d = pB - pA;
  float32 distSqr = b2Dot(d, d);
  float32 rA = circleA->m_radius, rB = circleB->m_radius;
  float32 radius = rA + rB;
  if (distSqr > radius * radius)
  {
    return;
  }

  manifold->type = b2Manifold::e_circles;
  manifold->localPoint = circleA->m_p;
  manifold->localNormal.SetZero();
  manifold->pointCount = 1;

  manifold->points[0].localPoint = circleB->m_p;
  manifold->points[0].id.key = 0;
  */
};

#endif