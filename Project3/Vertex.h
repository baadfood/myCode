#ifndef Vertex_h_
#define Vertex_h_

#include <glm/glm.hpp>

class Vertex
{
public:
  Vertex(glm::vec3 const & p_pos, glm::vec2 const & p_texCoord, glm::vec3 const & p_normal = glm::vec3(0,0,1));
  Vertex(Vertex const & p_other);
  Vertex const & operator=(Vertex const & p_other);
  virtual ~Vertex();
  
  glm::vec3 const & getPos() const;
  glm::vec3 & getPos();

  glm::vec2 const & getTexCoord() const;
  glm::vec2 & getTexCoord();

  glm::vec3 const & getNormal() const;
  glm::vec3 & getNormal();

private:
  glm::vec3 m_pos;
  glm::vec2 m_texCoord;
  glm::vec3 m_normal;
};

#endif