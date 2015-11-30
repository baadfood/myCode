#include "Transform2D.h"

Transform2D::Transform2D(glm::vec2 p_pos, float p_rot, glm::vec2 p_scale)
{
  m_pos = p_pos;
  m_rot = p_rot;
  m_scale = p_scale;
  m_z = 0;
}

Transform2D::~Transform2D()
{

}

glm::mat4 const & Transform2D::getModel()
{
  glm::mat4 posMatrix = glm::translate(glm::vec3(m_pos, m_z));
  glm::mat4 rotZMatrix = glm::rotate(m_rot, glm::vec3(0.0, 0.0, 1.0));
  glm::mat4 scaleMatrix = glm::scale(glm::vec3(m_scale, 1.0));
  m_model = posMatrix * rotZMatrix * scaleMatrix;

  return m_model;
}

glm::vec2 const & Transform2D::getPos() const
{
  return m_pos;
}

glm::vec2 & Transform2D::getPos()
{
  return m_pos;
}

float const & Transform2D::getRot() const
{
  return m_rot;
}

float & Transform2D::getRot()
{
  return m_rot;
}

glm::vec2 const & Transform2D::getScale() const
{
  return m_scale;
}

glm::vec2 & Transform2D::getScale()
{
  return m_scale;
}
