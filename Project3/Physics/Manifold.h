#ifndef Manifold_h_
#define Manifold_h_

#include <atomic>
#include <boost/lockfree/queue.hpp>
#include <glm/glm.hpp>
class Object;

class ManifoldPoint
{
public:
  Object * object;
  glm::i64vec2 contactPoint;
  glm::f64 normalImpulse;
  glm::f64 tangentImpulse;
};

class Manifold
{
  static boost::lockfree::queue<Manifold*> s_buffer;
public:
  enum EManifoldType
  {
    eCircles,
    ePolygons,
    eCircleToPolygon
  };

  static Manifold * newManifold()
  {
    Manifold * manifold;
    if (s_buffer.pop(manifold) == false)
    {
      manifold = new Manifold();
    }
    return manifold;
  }

  static void freeManifold(Manifold * p_manifold)
  {
    s_buffer.push(p_manifold);
  }

  glm::i64vec2 localPoint;
  glm::f64vec2 localNormal;
  EManifoldType type;
  ManifoldPoint contactPoints[2];
  unsigned short pointCount;
  glm::f64 penetration;
};


#endif
