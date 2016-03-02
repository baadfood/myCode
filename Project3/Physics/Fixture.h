#ifndef Fixture_h_
#define Fixture_h_

#include "ContactFilter.h"

#include <glm/glm.hpp>
#include <vector>

class Shape;
class Object;

class Fixture
{
public:
  Shape * shape;
  Object * object;

  glm::float32 restitution;
  glm::float32 friction;
  glm::float32 density;

  std::vector<ContactFilter> contactFilters;
};

#endif