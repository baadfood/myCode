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


  virtual ~Shape();

  virtual EType getType() { return m_type; }
  virtual AABB const & getAabb() { return m_aabb; }
  virtual void computeAabb(AABB & p_aabb, Transform2d const & p_transform) = 0;
  virtual void calculateMassData(MassData & p_massData, glm::float32 p_density) = 0;
  virtual bool TestPoint(const Transform2d& p_transform, const glm::i64vec2 & p_pos) const = 0;

/*
  virtual bool RayCast(b2RayCastOutput* output, const b2RayCastInput& input,
                       const b2Transform& transform, int32 childIndex) const = 0;
                       */
  EType m_type;
  AABB m_aabb;
};


#endif