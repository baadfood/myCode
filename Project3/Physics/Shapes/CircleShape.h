#ifndef CircleShape_h_
#define CircleShape_h_

#include "../Shape.h"
#include "../Transform.h"
#include <glm/glm.hpp>
#define _USE_MATH_DEFINES
#include <math.h>

class CircleShape : public Shape
{
public:
  CircleShape()
  {
    Shape::m_type = Shape::eCircle;
  }
  
  virtual ~CircleShape()
  {
  }

  virtual void computeAabb(AABB & p_aabb, Transform2d const & p_transform)
  {
    p_aabb.setCenter(p_transform.pos + p_transform.applyRotation(m_pos));
    p_aabb.setSize(glm::u64vec2(m_radius * 1.05, m_radius * 1.05));
  }
  virtual void calculateMassData(MassData & p_massData, glm::float32 p_density)
  {
    glm::dvec2 dpos =  static_cast<glm::dvec2>(m_pos);
    p_massData.mass = p_density * M_PI * m_radius * m_radius;
    p_massData.center = m_pos;
    p_massData.inertia = p_massData.mass * (0.5f * m_radius * m_radius + glm::dot(dpos, dpos));
  }
  virtual bool TestPoint(const Transform2d& p_transform, const glm::i64vec2 & p_pos) const
  {
    glm::dvec2 distance = static_cast<glm::dvec2>(p_pos - p_transform.pos + p_transform.applyRotation(m_pos));
    return glm::dot(distance, distance) <= m_radius * m_radius;
  }
};

#endif