#ifndef Contact_h_
#define Contact_h_

#include "Physics/Fixture.h"
#include "Physics/Manifold.h"

class Contact
{
public:
  Fixture fixtures[2];
  int vertexIndices[2];

  Manifold manifold;

  glm::f32 toi; // 0 -> 1 fraction of current step;

  float contactFriction;
  float contactRestitution;
};

#endif