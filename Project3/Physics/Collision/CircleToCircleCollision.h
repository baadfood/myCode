#ifndef CircleToCircleCollision_h_
#define CircleToCircleCollision_h_

#include "Collision.h"
#include "../Manifold.h"
#include "../Transform.h"
#include <glm/glm.hpp>
#include <complex>
#include "../Shapes/CircleShape.h"
#include <Object.h>
#include "Physics/Fixture.h"
#include "Physics/Contact.h"

class CircleToCircleCollision: public Collision
{
public:

  virtual bool Collide(Contact *& p_contact,
    Fixture & p_fix1, Transform2d const & p_tf1,
    Fixture & p_fix2, Transform2d const & p_tf2)
  {
    CircleShape * circle1 = static_cast<CircleShape *>(p_fix1.shape);
    CircleShape * circle2 = static_cast<CircleShape *>(p_fix2.shape);

    glm::i64vec2 circle1Pos = p_tf1.multiply(circle1->getPos());
    glm::i64vec2 circle2Pos = p_tf2.multiply(circle2->getPos());

    glm::f64vec2 delta = static_cast<glm::f64vec2>(circle2Pos - circle1Pos);
    glm::f64 distance = std::sqrt(glm::dot(delta, delta));
    glm::f64 limit = circle1->getRadius() + circle2->getRadius();
    if (distance > limit)
    {
      return false;
    }

    p_contact = new Contact;
    glm::f64vec2 normal = static_cast<glm::f64vec2>(circle2Pos - circle1Pos) / distance;

    p_contact->contactFriction = std::sqrt(p_fix1.friction * p_fix2.friction);
    p_contact->contactRestitution = std::sqrt(p_fix1.restitution * p_fix2.restitution);
    p_contact->manifold.pointCount = 0;
    p_contact->manifold.type = Manifold::eCircles;
    p_contact->manifold.localNormal = normal;
    p_contact->manifold.penetration = limit - distance;
    p_contact->manifold.pointCount = 1;
    p_contact->manifold.contactPoints[0].contactPoint = p_fix1.object->getPos() + glm::i64vec2(normal * (distance / 2));
    p_contact->relativeVelocity = (glm::f64vec2)p_fix1.object->getSpeed() + ((glm::f64vec2(circle1->getPos()) + normal * (glm::f64)circle1->getRadius()) * (glm::f64)p_fix1.object->getRotSpeed())
                                -((glm::f64vec2)p_fix2.object->getSpeed() + ((glm::f64vec2(circle2->getPos()) - normal * (glm::f64)circle2->getRadius()) * (glm::f64)p_fix2.object->getRotSpeed()));
    double speed = glm::length(p_contact->relativeVelocity);
    p_contact->timeOfImpact = p_contact->manifold.penetration / speed;

    p_contact->fixtures[0] = &p_fix1;
    p_contact->fixtures[1] = &p_fix2;

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