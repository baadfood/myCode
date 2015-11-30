#ifndef MassData_h_
#define MassData_h_

#include <glm/glm.hpp>

struct MassData
{
  glm::f32 mass;
  glm::f32 inertia;
  glm::i64vec2 center;
};

#endif