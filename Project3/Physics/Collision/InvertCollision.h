#ifndef InvertCollision_h_
#define InvertCollision_h_

#include "Physics/Collision/Collision.h"

#include <memory>

class InvertCollision: public Collision
{
public:
  InvertCollision(Collision * p_invert):
  invert(p_invert)
  {  }

  virtual bool Collide(Contact *& p_contact,
    Fixture & p_fix1, Transform2d const & p_tf1,
    Fixture & p_fix2, Transform2d const & p_tf2)
  {
    if(invert->Collide(p_contact, p_fix2, p_tf2, p_fix1, p_tf2))
    {
      return true;
    }
    return false;
  }

  std::unique_ptr<Collision> invert;
};

#endif