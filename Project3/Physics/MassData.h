#ifndef MassData_h_
#define MassData_h_

#include <glm/glm.hpp>

struct MassData
{
  glm::f32 mass;
  glm::f32 inertia;
  glm::f32vec2 center;
};

#endif