/*
#pragma once
#include "Shape.h"

#include <glm/glm.hpp>

class Circle :
  public Shape
{
public:
  const int CircleShapeType = 1;

  Circle();
  virtual ~Circle();

  void setRadius(glm::u64 p_radius) { m_radius = p_radius; m_boundary.setSize(glm::u64vec2(p_radius, p_radius)); }
  glm::u64 const & getRadius() const { return m_radius; }

  virtual int getType() { return CircleShapeType; }

protected:
  glm::u64 m_radius;
};

*/