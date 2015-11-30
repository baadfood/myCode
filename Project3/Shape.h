#pragma once

#include <glm\glm.hpp>
#include "AABB.h"

class Shape
{
public:
  const int ShapeBaseType = 0;

  Shape();
  virtual ~Shape();

  inline glm::i64vec2 const & getPos() const { return m_pos; }
  inline void setPos(glm::i64vec2 const & p_pos) { m_pos = p_pos; m_boundary.setCenter(m_pos); }

  inline glm::f32 const & getMass() const { return m_invMass; }
  inline void setMass(glm::f32 p_invMass) { m_invMass = p_invMass; }

  inline AABB const & getAabb() const { return m_boundary; }
  inline void setAabb(AABB const & p_boundary) { m_boundary = p_boundary; }

  virtual int getType() { return ShapeBaseType; }

protected:

  AABB m_boundary;

  glm::f32 m_invMass; //   1/mass 
  glm::i64vec2 m_pos;
};

