#ifndef CircleToPolygonCollision_h_
#define CircleToPolygonCollision_h_

#include "Collision.h"
#include "../Manifold.h"
#include "../Transform.h"
#include <glm/glm.hpp>
#include <complex>
#include "../Shapes/CircleShape.h"
#include "../Shapes/PolygonShape.h"
#include <Object.h>
#include "Physics/Fixture.h"
#include "Physics/Contact.h"

class CircleToPolygonCollision: public Collision
{
public:

  virtual bool Collide(Contact *& p_contact,
    Fixture & p_fix1, Transform2d const & p_tf1,
    Fixture & p_fix2, Transform2d const & p_tf2)
  {
    CircleShape * circle = static_cast<CircleShape *>(p_fix1.shape);
    PolygonShape * polygon = static_cast<PolygonShape *>(p_fix2.shape);

    glm::i64vec2 circlePos = p_tf1.multiply(circle->getPos());
    circlePos = p_tf2.revertMultiply(circlePos);

    glm::f64 biggestSeparation = -std::numeric_limits<glm::f64>::max();
    
    glm::f64 circleRadius = circle->getRadius();
    unsigned int faceNormal = 0;
    for(unsigned int index = 0;
        index < polygon->vertexCount;
        index++)
    {
      glm::f64 faceSeparation = glm::dot(polygon->normals[index], glm::f64vec2(circlePos) - polygon->vertices[index]);
      
      if(faceSeparation > circleRadius)
      {
        return false;
      }
      
      if(faceSeparation > biggestSeparation)
      {
        faceNormal = index;
        biggestSeparation = faceSeparation;
      }
    }
    
    glm::i64vec2 faceVert1 = polygon->vertices[faceNormal];
    if(++faceNormal == polygon->vertexCount) faceNormal = 0;
    glm::i64vec2 faceVert2 = polygon->vertices[faceNormal];

    p_contact = Contact::newContact();
    p_contact->contactFriction = std::sqrt(p_fix1.friction * p_fix2.friction);
    p_contact->contactRestitution = std::sqrt(p_fix1.restitution * p_fix2.restitution);
    p_contact->manifold.pointCount = 0;
    p_contact->manifold.type = Manifold::eCircleToPolygon;
    p_contact->manifold.penetration = circle->getRadius() - biggestSeparation;
    
    if(biggestSeparation <= 0)
    {
      // Circle is inside polygon
      p_contact->manifold.pointCount = 1;
      p_contact->manifold.localNormal = -(p_tf2.multiply(polygon->normals[faceNormal]));
      p_contact->manifold.contactPoints[0].contactPoint = p_contact->manifold.localNormal * glm::f64(circle->getRadius()) + glm::f64vec2(p_tf1.multiply(circle->getPos()));
      p_contact->manifold.penetration = circle->getRadius();
      p_contact->relativeVelocity = (glm::f64vec2)p_fix1.object->getSpeed() + ((glm::f64vec2(circle->getPos()) + p_contact->manifold.localNormal * (glm::f64)circle->getRadius()) * (glm::f64)p_fix1.object->getRotSpeed())
                                  -((glm::f64vec2)p_fix2.object->getSpeed() + ((glm::f64vec2(polygon->getPos()) - (glm::f64vec2)faceVert1 * (glm::f64)p_fix2.object->getRotSpeed())));
    }
    else
    {

      
      glm::i64vec2 toDot1 = circlePos - faceVert1;
      glm::i64vec2 toDot2 = circlePos - faceVert2;

      glm::f64 dot1 = glm::dot(glm::f64vec2(toDot1), glm::f64vec2(faceVert2 - faceVert1));
      glm::f64 dot2 = glm::dot(glm::f64vec2(toDot2), glm::f64vec2(faceVert1 - faceVert2));
      
      glm::f64 radiusSqr = circle->getRadius() * circle->getRadius();
      
      double speed = glm::length(p_contact->relativeVelocity);


      if(dot1 <= 0.0)
      {
        if(toDot1.x * toDot1.x + toDot1.y * toDot1.y > radiusSqr)
        {
          Contact::freeContact(p_contact);
          return false;
        }
        
        p_contact->manifold.pointCount = 1;
        p_contact->manifold.contactPoints[0].contactPoint = p_tf2.multiply(faceVert1);
        p_contact->manifold.localNormal = glm::normalize(p_tf2.applyRotation(glm::f64vec2(faceVert1 - circlePos)));
        p_contact->relativeVelocity = (glm::f64vec2)p_fix1.object->getSpeed() + ((glm::f64vec2(circle->getPos()) + p_contact->manifold.localNormal * (glm::f64)circle->getRadius()) * (glm::f64)p_fix1.object->getRotSpeed())
                                    -((glm::f64vec2)p_fix2.object->getSpeed() + ((glm::f64vec2(polygon->getPos()) - (glm::f64vec2)faceVert1 * (glm::f64)p_fix2.object->getRotSpeed())));
      }
      
      else if(dot2 <= 0.0)
      {
        if(toDot2.x * toDot2.x + toDot2.y * toDot2.y > radiusSqr)
        {
          Contact::freeContact(p_contact);
          return false;
        }
        
        p_contact->manifold.pointCount = 1;
        p_contact->manifold.contactPoints[0].contactPoint = p_tf2.multiply(faceVert2);
        p_contact->manifold.localNormal = glm::normalize(p_tf2.applyRotation(glm::f64vec2(faceVert2 - circlePos)));
        p_contact->relativeVelocity = (glm::f64vec2)p_fix1.object->getSpeed() + ((glm::f64vec2(circle->getPos()) + p_contact->manifold.localNormal * (glm::f64)circle->getRadius()) * (glm::f64)p_fix1.object->getRotSpeed())
                                    -((glm::f64vec2)p_fix2.object->getSpeed() + ((glm::f64vec2(polygon->getPos()) - (glm::f64vec2)faceVert2 * (glm::f64)p_fix2.object->getRotSpeed())));
      }
      
      else
      {
        glm::f64vec2 normal = polygon->normals[faceNormal];
        if(glm::dot(glm::f64vec2(circlePos - faceVert1), normal) > circleRadius)
        {
          Contact::freeContact(p_contact);
          return false;
        }
        normal = -p_tf2.applyRotation(normal);
        p_contact->manifold.localNormal = normal;
        p_contact->manifold.pointCount = 1;
        p_contact->manifold.contactPoints[0].contactPoint = glm::i64vec2(normal * circleRadius) + circlePos;
        glm::f64vec2 polyPoint = glm::f64vec2(faceVert1);
        polyPoint += glm::normalize(glm::f64vec2(faceVert2 - faceVert1)) * dot1;
        p_contact->relativeVelocity = (glm::f64vec2)p_fix1.object->getSpeed() + ((glm::f64vec2(circle->getPos()) + p_contact->manifold.localNormal * (glm::f64)circle->getRadius()) * (glm::f64)p_fix1.object->getRotSpeed())
                                    -((glm::f64vec2)p_fix2.object->getSpeed() + ((glm::f64vec2(polygon->getPos()) - (glm::f64vec2)polyPoint * (glm::f64)p_fix2.object->getRotSpeed())));
      }
    }

    p_contact->timeOfImpact = p_contact->manifold.penetration / glm::length(p_contact->relativeVelocity);
    p_contact->fixtures[0] = &p_fix1;
    p_contact->fixtures[1] = &p_fix2;


    return true;
  }
};

#endif