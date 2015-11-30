#ifndef Collision_h_
#define Collision_h_

#include "Physics/Fixture.h"
#include "Physics/Contact.h"

struct Transform2d;

class Collision
{
public:
  virtual ~Collision() {}

  virtual bool Collide(Contact* manifold,
    Fixture const & p_fix1, Transform2d const & p_tr1,
    Fixture const & p_fix2, Transform2d const & p_tr2) = 0;
};

#endif