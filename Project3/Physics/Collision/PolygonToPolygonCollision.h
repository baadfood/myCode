#ifndef PolygonToPolygonCollision_h_
#define PolygonToPolygonCollision_h_

#include "Collision.h"
#include "Physics/Shapes/PolygonShape.h"

namespace
{
  double FindAxisLeastPenetration(unsigned int & p_face, PolygonShape * p_poly1, PolygonShape * p_poly2, Transform2d const & p_tr1, Transform2d const & p_tr2)
  {
    float bestDistance = -std::numeric_limits<float>::max();
    for (unsigned int index = 0;
    index < p_poly1->vertexCount;
      index++)
    {
      glm::f64vec2 normal = p_poly1->normals[index];
      glm::f64vec2 worldNormal = p_tr1.applyRotation(normal);
      normal = p_tr2.revertRotation(worldNormal);

      glm::f64vec2 support = p_poly2->getSupport(-normal);

      glm::f64vec2 vertex = p_poly1->vertices[index];

      vertex = p_tr1.multiply(vertex);
      vertex = p_tr2.revertMultiply(vertex);

      double distance = glm::dot(normal, support - vertex);
      if (distance > bestDistance)
      {
        bestDistance = distance;
        p_face = index;
      }
    }
    return bestDistance;
  }

  void FindIncidentFace(glm::f64vec2 * p_faces, PolygonShape * p_refPoly, Transform2d const & p_refTr, PolygonShape * p_incPoly, Transform2d const & p_incTr, unsigned int p_refIndex)
  {
    glm::f64vec2 referenceNormal = p_refPoly->normals[p_refIndex];

    referenceNormal = p_refTr.applyRotation(referenceNormal);
    referenceNormal = p_incTr.revertRotation(referenceNormal);

    unsigned int incidentFace = 0;
    double minDot = std::numeric_limits<double>::max();
    for (int index = 0;
    index < p_incPoly->vertexCount;
      index++)
    {
      double dot = glm::dot(referenceNormal, p_incPoly->normals[index]);
      if (dot < minDot)
      {
        minDot = dot;
        incidentFace = index;
      }
    }

    p_faces[0] = p_incTr.multiply(p_incPoly->vertices[incidentFace]);
    incidentFace = incidentFace + 1 >= p_incPoly->vertexCount ? 0 : incidentFace + 1;
    p_faces[1] = p_incTr.multiply(p_incPoly->vertices[incidentFace]);
  }

  unsigned int clip(glm::f64vec2 const & p_normal, double p_c, glm::f64vec2 * p_face)
  {
    unsigned int sp = 0;
    glm::f64vec2 out[2] = { p_face[0], p_face[1] };

    glm::f64 d1 = glm::dot(p_normal, p_face[0]) - p_c;
    glm::f64 d2 = glm::dot(p_normal, p_face[1]) - p_c;

    if (d1 <= 0) out[sp++] = p_face[0];
    if (d2 <= 0) out[sp++] = p_face[1];

    if (d1 * d2 < 0.0)
    {
      glm::f64 alpha = d1 / (d1 - d2);
      out[sp] = p_face[0] + alpha * (p_face[1] - p_face[0]);
      ++sp;
    }

    p_face[0] = out[0];
    p_face[1] = out[1];

    assert(sp != 3);

    return sp;
  }
}

class PolygonToPolygonCollision: public Collision
{
public:
  virtual bool Collide(Contact*& p_contact,
    Fixture const & p_fix1, Transform2d const & p_tr1,
    Fixture const & p_fix2, Transform2d const & p_tr2)
  {
    PolygonShape * poly1 = reinterpret_cast<PolygonShape*>(p_fix1.shape);
    PolygonShape * poly2 = reinterpret_cast<PolygonShape*>(p_fix2.shape);

    unsigned int face1 = 666;
    double penetration1 = FindAxisLeastPenetration(face1, poly1, poly2, p_tr1, p_tr2);
    if (penetration1 >= 0)
    {
      // not colliding
      return false;
    }

    unsigned int face2 = 666;
    double penetration2 = FindAxisLeastPenetration(face2, poly2, poly1, p_tr2, p_tr1);
    if (penetration2 >= 0)
    {
      // not colliding
      return false;
    }

    unsigned int refIndex;
    bool flip;

    PolygonShape * refPoly;
    PolygonShape * incidentPoly;
    Transform2d const * refTr;
    Transform2d const * incTr;

    if (penetration1 >= penetration2)
    {
      refPoly = poly1;
      refTr = &p_tr1;
      incTr = &p_tr2;
      incidentPoly = poly2;
      refIndex = face1;
      flip = false;
    }
    else
    {
      refPoly = poly2;
      refTr = &p_tr2;
      incTr = &p_tr1;
      incidentPoly = poly1;
      refIndex = face2;
      flip = true;
    }

    glm::f64vec2 incidentFace[2];

    FindIncidentFace(incidentFace, refPoly, *refTr, incidentPoly, *incTr, refIndex);

    glm::f64vec2 vertex1 = refPoly->vertices[refIndex];
    if (++refIndex == refPoly->vertexCount) refIndex = 0;
    glm::f64vec2 vertex2 = refPoly->vertices[refIndex];

    vertex1 = refTr->multiply(vertex1);
    vertex2 = refTr->multiply(vertex2);

    glm::f64vec2 sidePlaneNormal = vertex2 - vertex1;
    sidePlaneNormal = glm::normalize(sidePlaneNormal);

    glm::f64vec2 refFaceNormal(sidePlaneNormal.y, -sidePlaneNormal.x);

    glm::f64 refC = glm::dot(refFaceNormal, vertex1);
    glm::f64 negSide = -glm::dot(sidePlaneNormal, vertex1);
    glm::f64 posSide = glm::dot(sidePlaneNormal, vertex2);

    if (clip(-sidePlaneNormal, negSide, incidentFace) < 2)
    {
      // floating point error
      return false;
    }
    if (clip(sidePlaneNormal, posSide, incidentFace) < 2)
    {
      // floating point error
      return false;
    }

    // collision

    p_contact = new Contact;
    p_contact->manifold.localNormal = flip ? -refFaceNormal : refFaceNormal;
    p_contact->manifold.penetration = 0;

    unsigned int contactPoints = 0;
    glm::f64 separation = glm::dot(refFaceNormal, incidentFace[0]) - refC;
    if (separation <= 0)
    {
      p_contact->manifold.contactPoints[0].contactPoint = incidentFace[0];
      p_contact->manifold.penetration -= separation;
      ++contactPoints;
    }

    separation = glm::dot(refFaceNormal, incidentFace[1]) - refC;
    if (separation <= 0)
    {
      p_contact->manifold.contactPoints[contactPoints].contactPoint = incidentFace[1];
      p_contact->manifold.penetration -= separation;
      ++contactPoints;
      if (contactPoints != 1)
      {
        p_contact->manifold.penetration /= double(contactPoints);
      }
    }

    p_contact->manifold.pointCount = contactPoints;


    p_contact->contactFriction = std::sqrt(p_fix1.friction * p_fix2.friction);
    p_contact->contactRestitution = std::sqrt(p_fix1.restitution * p_fix2.restitution);
    p_contact->manifold.type = Manifold::ePolygons;
    p_contact->relativeVelocity = (glm::f64vec2)p_fix1.object->getSpeed() + ((glm::f64vec2(poly1->getPos()) + p_contact->manifold.localNormal * p_contact->manifold.penetration) * (glm::f64)p_fix1.object->getRotSpeed())
                               - ((glm::f64vec2)p_fix2.object->getSpeed() + ((glm::f64vec2(poly2->getPos()) - p_contact->manifold.localNormal * p_contact->manifold.penetration) * (glm::f64)p_fix2.object->getRotSpeed()));
    double speed = glm::length(p_contact->relativeVelocity);
    p_contact->timeOfImpact = p_contact->manifold.penetration / speed;


    p_contact->fixtures[0].object = p_fix1.object;
    p_contact->fixtures[1].object = p_fix2.object;
    p_contact->fixtures[0].shape = p_fix1.shape;
    p_contact->fixtures[1].shape = p_fix2.shape;



    return true;
  }


};

#endif
