#include "Vertex.h"

#include <glm/glm.hpp>

Vertex::Vertex(glm::vec3 const & p_pos, glm::vec2 const & p_texCoord, glm::vec3 const & p_normal)
{
  m_pos = p_pos;
  m_texCoord = p_texCoord;
  m_normal = p_normal;
}

Vertex::~Vertex()
{
}

Vertex::Vertex(Vertex const & p_other)
{
  m_pos = p_other.m_pos;
  m_texCoord = p_other.m_texCoord;
  m_normal = p_other.m_normal;
}

Vertex const & Vertex::operator=(Vertex const & p_other)
{
  if (&p_other == this)
  {
    return *this;
  }

  m_pos = p_other.m_pos;
  m_texCoord = p_other.m_texCoord;
  m_normal = p_other.m_normal;
  return *this;
}

glm::vec3 const & Vertex::getPos() const
{
  return m_pos;
}

glm::vec3 & Vertex::getPos()
{
  return m_pos;
}

glm::vec2 const & Vertex::getTexCoord() const
{
  return m_texCoord;
}

glm::vec2 & Vertex::getTexCoord()
{
  return m_texCoord;
}

glm::vec3 const & Vertex::getNormal() const
{
  return m_normal;
}

glm::vec3 & Vertex::getNormal()
{
  return m_normal;
}
