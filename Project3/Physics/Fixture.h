#ifndef Fixture_h_
#define Fixture_h_

#include <glm/glm.hpp>
#include <vector>
#include <boost/concept_check.hpp>

class Shape;
class Object;

class Fixture
{
public:
  enum EFixtureType
  {
    eNormal,    // normal physical fixture, collides
    eSensor     // interacts with eNormal, doesn't cause collision response
  };

  Shape * shape;
  Object * object;
  EFixtureType type;

  glm::float32 restitution;
  glm::float32 friction;
  glm::float32 density;
  
  inline static bool fixturesInteract(Fixture const & p_fix1, Fixture const & p_fix2)
  {
    if(p_fix1.type != eSensor
    || p_fix2.type != eSensor)
    {
      return true;
    }
    return false;
  }
  inline static bool fixturesCollide(Fixture const & p_fix1, Fixture const & p_fix2)
  {
    if(p_fix1.type == eNormal
    && p_fix2.type == eNormal)
    {
      return true;
    }
    return false;
  }
};

#endif