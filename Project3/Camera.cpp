#include "Camera.h"

Camera::Camera(glm::vec3 const & p_pos, float p_fov, float p_aspect, float p_zNear, float p_zFar)
{
  m_perspective = glm::perspective(p_fov, p_aspect, p_zNear, p_zFar);
  m_position = p_pos;
  m_forward = glm::vec3(0.0, 0.0, 1.0);
  m_up = glm::vec3(0.0, 1.0, 0.0);
}

glm::mat4 Camera::getViewProjection() const
{
  return m_perspective * glm::lookAt(m_position, m_position + m_forward, m_up);
}

Camera::~Camera()
{
}

AABB const & Camera::getAABB() const
{
  return m_aabb;
}

void Camera::moveBy(glm::vec3 const & p_pos)
{
  m_position += p_pos;
  glm::dvec4 topRight = static_cast<glm::dmat4>(glm::inverse(m_perspective * glm::lookAt(m_position, m_position + m_forward, m_up))) * glm::dvec4(1, 1, -m_position.z, 1);
  topRight /= topRight.w;
  glm::dvec4 botLeft = static_cast<glm::dmat4>(glm::inverse(m_perspective * glm::lookAt(m_position, m_position + m_forward, m_up))) * glm::dvec4(-1, -1, -m_position.z, 1);
  botLeft /= botLeft.w;
  m_aabb.setCenter(glm::i64vec2((topRight.x + botLeft.x) / 2, (topRight.y + botLeft.y) / 2));
  m_aabb.setSize(glm::u64vec2(topRight.x - botLeft.x, topRight.y - botLeft.y));
}

void Camera::moveTo(glm::vec3 const & p_pos)
{
  m_position = p_pos;
}

glm::vec3 const & Camera::getPosition() const
{
  return m_position;
}

glm::i64vec2 Camera::screenToWorld(glm::i32vec2 const & p_screen)
{
  return glm::i64vec2(0, 0);
}

void Camera::lookAt(Object * p_object)
{
  // do something nice
}
