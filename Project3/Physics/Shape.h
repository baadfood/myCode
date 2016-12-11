#ifndef Shape_h_
#define Shape_h_

#include "../AABB.h"
#include "Transform.h"
#include "MassData.h"

class Shape
{
public:
  enum EType
  {
    eCircle = 0,
    eEdge = 1,
    ePolygon = 2,
    eChain = 3,
    eTypeCount = 4
  };

  Shape():
  m_pos(0,0),
  m_radius(0)
  {
  }
  
  virtual ~Shape()
  {
  }

  glm::i64vec2 getPos() const { return m_pos; }
  glm::u64 getRadius() const { return m_radius; }

  virtual EType getType() { return m_type; }
  virtual AABB const & getAabb() const { return m_aabb; }
  virtual AABB & getAabb() { return m_aabb; }
  virtual void computeAabb(AABB & p_aabb, Transform2d const & p_transform) = 0;
  virtual void calculateMassData(MassData & p_massData, glm::float32 p_density, Transform2d const & p_transform) = 0;
protected:

  glm::i64vec2 m_pos;
  glm::u64 m_radius;
  EType m_type;
  AABB m_aabb;
};


#endif