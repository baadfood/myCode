#ifndef Transform_h_
#define Transform_h_

#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>

class Transform
{
public:
  Transform(glm::vec3 const & p_pos = glm::vec3(), glm::vec3 const & p_rot = glm::vec3(), glm::vec3 const & p_scale = glm::vec3(1.0f, 1.0f, 1.0f));
  virtual ~Transform();

  glm::mat4 getModel() const;

  glm::vec3 const & getPos() const;
  glm::vec3 & getPos();

  glm::vec3 const & getRot() const;
  glm::vec3 & getRot();

  glm::vec3 const & getScale() const;
  glm::vec3 & getScale();

private:
  struct Private;
  Private * d;
};

#endif

