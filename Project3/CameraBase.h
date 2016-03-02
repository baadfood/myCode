#ifndef CameraBase_h_
#define CameraBase_h_

#include <glm/glm.hpp>
#include "AABB.h"
#include "Object.h"

class CameraBase: public Object
{
public:
  CameraBase() {}
  virtual glm::i64vec2 screenToWorld(glm::i32vec2 const & p_screen) = 0;
  virtual void lookAt(Object * p_object) = 0;
};

#endif