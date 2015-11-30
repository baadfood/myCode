#ifndef Camera_h_
#define Camera_h_

#include "CameraBase.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "AABB.h"

class Camera : public CameraBase
{
public:
  Camera(glm::vec3 const & p_pos, float p_fov, float p_aspect, float p_zNear, float p_zFar);
  virtual ~Camera();

  glm::mat4 getViewProjection() const;

  void moveBy(glm::vec3 const & p_pos);
  void moveTo(glm::vec3 const & p_pos);

  glm::vec3 const & getPosition() const;

  AABB const & getAABB() const;

  virtual glm::i64vec2 screenToWorld(glm::i32vec2 const & p_screen);
  virtual void lookAt(Object * p_object);

protected:
private:
  glm::mat4 m_perspective;
  glm::vec3 m_position;
  glm::vec3 m_forward;
  glm::vec3 m_up;
  AABB m_aabb;
};

#endif
