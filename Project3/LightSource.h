#ifndef LightSource_h_
#define LightSource_h_

#include <glm\glm.hpp>

struct LightSource
{
  glm::vec3 position;
  glm::vec3 intensities;
  float attenuation;
  float ambienCoefficient;
};

#endif

