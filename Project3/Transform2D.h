#pragma once
#define GLM_FORCE_RADIANS
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>

class Transform2D
{
public:
  Transform2D(glm::vec2 p_pos = glm::vec2(0,0), float p_rot = 0, glm::vec2 p_scale = glm::vec2(1, 1));
  virtual ~Transform2D();

  glm::mat4 const & getModel();

  glm::vec2 const & getPos() const;
  glm::vec2 & getPos();

  float const & getRot() const;
  float & getRot();

  glm::vec2 const & getScale() const;
  glm::vec2 & getScale();
private:
  glm::mat4 m_model;
  glm::vec2 m_pos;
  float m_rot;
  glm::vec2 m_scale;
  float m_z;
};

