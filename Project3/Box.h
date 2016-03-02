/*

#pragma once
#include "Shape.h"
class Box :
  public Shape
{
public:
  const int CircleShapeType = 1;
  Box();
  virtual ~Box();

  inline void setRot(glm::float32 p_rot) { m_rot = p_rot; updateAabb(); }
  inline glm::float32 getRot() const { return m_rot; }

  inline void setHalfSize(glm::u64vec2 p_size) { m_halfSize = p_size; updateAabb(); }
  inline glm::u64vec2 getHalfSize() { return m_halfSize; }

  virtual int getType() { return CircleShapeType; }
protected:
  inline void updateAabb()
  {
    float cosRot = cos(m_rot);
    float sinRot = sin(m_rot);

    // TODO sin cos no need for precision;
    m_boundary.setSize(glm::u64vec2(cosRot * m_halfSize.x + sinRot * m_halfSize.y, sinRot * m_halfSize.x + cosRot * m_halfSize.y));
  }

  glm::float32 m_rot;
  glm::u64vec2 m_halfSize;
};

*/