#pragma once

#include "Object.h"
#include "Physics/Fixture.h"
#include "Physics/Shape.h"
#include "Physics/Manifold.h"
#include "utils.h"
#include "Components/Component.h"

class Contact
{
public:
  Component * components[2];
  Fixture * fixtures[2];
  int vertexIndices[2];

  Manifold manifold;

  glm::f32 timeOfImpact; // 0 -> 1 fraction of current step;

  glm::f64 contactFriction;
  glm::f64 contactRestitution;
  glm::f64vec2 relativeVelocity;

  glm::f64 posCorrection;
  glm::f64vec2 posCorrectionVec;
  
  bool physical = true;

  void applyImpulse()
  {
    if(physical == false)
    {
      return;
    }
    Object * obj1 = fixtures[0]->object;
    Object * obj2 = fixtures[1]->object;
    glm::f64vec2 obj1Speed((glm::f64vec2)(obj1->getSpeed()));
    glm::f64vec2 obj2Speed((glm::f64vec2)(obj2->getSpeed()));
    glm::f64 obj1RotSpeed = obj1->getRotSpeed();
    glm::f64 obj2RotSpeed = obj2->getRotSpeed();

    glm::i64vec2 shape1Pos = obj1->getPos() + fixtures[0]->shape->getPos();
    glm::i64vec2 shape2Pos = obj2->getPos() + fixtures[1]->shape->getPos();

    for(unsigned short i = 0; i < manifold.pointCount; ++i)
    {
      // Calculate radii from COM to contact
      glm::f64vec2 radiiObj1 = (glm::f64vec2)(manifold.contactPoints[i].contactPoint - obj1->getPos());
      glm::f64vec2 radiiObj2 = (glm::f64vec2)(manifold.contactPoints[i].contactPoint - obj2->getPos());

      // Relative velocity
      glm::f64vec2 obj1Vel = obj1Speed + mika::crossS(obj1RotSpeed, radiiObj1);
      glm::f64vec2 obj2Vel = obj2Speed + mika::crossS(obj2RotSpeed, radiiObj2);

      glm::f64vec2 velocity;
      velocity = obj2Vel - obj1Vel;
      glm::f64vec2 positionRelation = glm::normalize(glm::f64vec2(shape2Pos - shape1Pos));

            // Relative velocity along the normal
      glm::f64 contactVel = glm::dot(velocity, manifold.localNormal);

      // Do not resolve if velocities are separating
      if (contactVel >= 0
      ||  contactVel != contactVel)
      {
        return;
      }

      glm::f64 raCross1 = mika::cross(radiiObj1, manifold.localNormal);
      glm::f64 raCross2 = mika::cross(radiiObj2, manifold.localNormal);
      glm::f64 invMassSum = obj1->getInvMass() + obj2->getInvMass() + raCross1 * raCross1 * obj1->getInvInertia() + raCross2 * raCross2 * obj2->getInvInertia();

      // Calculate impulse scalar
      glm::f64 impulseScalar = -(1.0f + contactRestitution) * contactVel;
      impulseScalar /= invMassSum;
      impulseScalar /= (glm::f64)manifold.pointCount;

      static glm::f64 biggestImpulseScalar = 0;
      if (impulseScalar > biggestImpulseScalar)
      {
        biggestImpulseScalar = impulseScalar;
        std::cout << "New biggest impulse " << impulseScalar << std::endl;
      }

      // Apply impulse
      glm::f64vec2 impulse = manifold.localNormal * impulseScalar;
      obj1->applyImpulse( -impulse, radiiObj1 );
      obj2->applyImpulse( impulse, radiiObj2 );

      glm::f64vec2 t = velocity - (manifold.localNormal * glm::dot(velocity, manifold.localNormal));
      if (glm::length(t) == 0)
      {
        // HEADSHOT, going directly along the normal.
        // This means no tangential velocity.
        return;
      }
      t = glm::normalize(t);

      // j tangent magnitude
      glm::f64 tangentScalar = -glm::dot(velocity, t);
      tangentScalar /= invMassSum;
      tangentScalar /= (glm::f64)manifold.pointCount;

      // Don't apply tiny friction impulses
      if(tangentScalar == 0)
      {
        return;
      }

      // Coulumb's law
      glm::f64vec2 tangentImpulse;
      if(std::abs( tangentScalar ) < impulseScalar * contactFriction)
        tangentImpulse = t * tangentScalar;
      else
        tangentImpulse = t * -impulseScalar * contactFriction;

      // Apply friction impulse
      obj1->applyImpulse( -tangentImpulse, radiiObj1 );
      obj2->applyImpulse(  tangentImpulse, radiiObj2 );
    }
  }

  void positionCorrectionPre()
  {
    if(physical == false)
    {
      return;
    }

    const float c_slop = OBJTOWORLD/16;
    const float c_correctionAmount = 0.5;
    Object * obj1 = fixtures[0]->object;
    Object * obj2 = fixtures[1]->object;

    posCorrectionVec = (std::max(manifold.penetration - c_slop, 0.0) / (obj1->getInvMass() + obj2->getInvMass())) * c_correctionAmount * manifold.localNormal;

    posCorrection = glm::length(posCorrectionVec);

    obj1->positionCorrectionPressure() += posCorrection;
    obj2->positionCorrectionPressure() += posCorrection;
  }

  void positionCorrection()
  {
    if(physical == false)
    {
      return;
    }

    if (posCorrection <= 0.0)
    {
      return;
    }
    glm::f64 pressure1 = fixtures[0]->object->positionCorrectionPressure();
    glm::f64 pressure2 = fixtures[1]->object->positionCorrectionPressure();

    glm::f64 totalPressure =  pressure1 + pressure2;
    glm::f64 pressureDif1 = (pressure2 / totalPressure) * 2;
    glm::f64 pressureDif2 = (pressure1 / totalPressure) * 2;

    fixtures[0]->object->moveBy(-posCorrectionVec * fixtures[0]->object->getInvMass() * pressureDif1);
    fixtures[1]->object->moveBy(posCorrectionVec * fixtures[1]->object->getInvMass() / pressureDif1);
  }
};

