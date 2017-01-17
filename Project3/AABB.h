#ifndef AABB_h_
#define AABB_h_

#include <glm/glm.hpp>
#include <iostream>
#include "Directions.h"
#include <cassert>
#include <sstream>

struct AABB
{
  AABB(glm::i64vec2 p_pos = glm::i64vec2(0,0), glm::u64vec2 p_halfSize = glm::u64vec2(0, 0)):
  m_pos(p_pos),
  m_halfSize(p_halfSize)
  {}

  ~AABB() {}

  inline void reset()
  {
    m_pos.x = 0;
    m_pos.y = 0;
    m_halfSize.x = 0;
    m_halfSize.y = 0;
  }

  inline AABB & operator+=(AABB const & p_other)
  {
    if(m_halfSize.x == 0
    && m_halfSize.y == 0)
    {
      m_pos = p_other.m_pos;
      m_halfSize = p_other.m_halfSize;
      return *this;
    }
    if(p_other.m_halfSize.x == 0
    && p_other.m_halfSize.y == 0)
    {
      return *this;
    }

    glm::i64vec2 low;
    glm::i64vec2 high;
    
    low.x = std::min(m_pos.x - glm::i64(m_halfSize.x), p_other.m_pos.x - glm::i64(p_other.m_halfSize.x));
    low.y = std::min(m_pos.y - glm::i64(m_halfSize.y), p_other.m_pos.y - glm::i64(p_other.m_halfSize.y));
    high.x = std::max(m_pos.x + glm::i64(m_halfSize.x), p_other.m_pos.x + glm::i64(p_other.m_halfSize.x));
    high.y = std::max(m_pos.y + glm::i64(m_halfSize.y), p_other.m_pos.y + glm::i64(p_other.m_halfSize.y));

    m_halfSize = high - low;
    m_halfSize /= 2;
    m_pos = static_cast<glm::i64vec2>(m_halfSize) + low;
  }

  inline void setCenter(glm::i64vec2 p_pos)
  {

    // maybe some sanity check to see that we don't go out of bounds?
    m_pos = p_pos;
  }
  inline void setSize(glm::u64vec2 p_halfSize)
  {
    // maybe some sanity check to see that we don't go out of bounds?
    m_halfSize = p_halfSize;
  }

  inline double getArea() const
  {
    return double(m_halfSize.x) * m_halfSize.y;
  }

  glm::i64vec2 getEdge(EDirection p_direction)
  {
    switch(p_direction)
    {
      case eN:
      return glm::i64vec2(m_pos.x, m_pos.y + m_halfSize.y);
      break;
      case eNe:
      return glm::i64vec2(m_pos.x + m_halfSize.x, m_pos.y + m_halfSize.y);
      break;
      case eE:
      return glm::i64vec2(m_pos.x + m_halfSize.x, m_pos.y);
      break;
      case eSe:
      return glm::i64vec2(m_pos.x + m_halfSize.x, m_pos.y - m_halfSize.y);
      break;
      case eS:
      return glm::i64vec2(m_pos.x, m_pos.y - m_halfSize.y);
      break;
      case eSw:
      return glm::i64vec2(m_pos.x - m_halfSize.x, m_pos.y - m_halfSize.y);
      break;
      case eW:
      return glm::i64vec2(m_pos.x - m_halfSize.x, m_pos.y);
      break;
      case eNw:
      return glm::i64vec2(m_pos.x - m_halfSize.x, m_pos.y + m_halfSize.y);
      break;
      default:
      // Why do you do this to me...
      assert(false);
      break;
    }
    return glm::i64vec2(0, 0);
  }

  EDirection getDirectionTo(AABB const & p_other) const
  {
    if(m_pos.x < p_other.m_pos.x)
    {
      if(m_pos.y < p_other.m_pos.y)
      {
        return eNe;
      }
      else if(m_pos.y > p_other.m_pos.y)
      {
        return eSe;
      }
      else
      {
        return eE;
      }
    }
    if(m_pos.x > p_other.m_pos.x)
    {
      if(m_pos.y < p_other.m_pos.y)
      {
        return eNw;
      }
      else if(m_pos.y > p_other.m_pos.y)
      {
        return eSw;
      }
      else
      {
        return eW;
      }
    }
    if(m_pos.y < p_other.m_pos.y)
    {
      return eN;
    }
    else if(m_pos.y > p_other.m_pos.y)
    {
      return eS;
    }
    // same exact spot..
    return eInvalidDirection;
  }

  inline bool contains(AABB const & p_other) const
  {
    if(glm::i64(m_pos.x + m_halfSize.x) >= glm::i64(p_other.m_pos.x + p_other.m_halfSize.x)
    && glm::i64(m_pos.x - m_halfSize.x) <= glm::i64(p_other.m_pos.x - p_other.m_halfSize.x)
    && glm::i64(m_pos.y + m_halfSize.y) >= glm::i64(p_other.m_pos.y + p_other.m_halfSize.y)
    && glm::i64(m_pos.y - m_halfSize.y) <= glm::i64(p_other.m_pos.y - p_other.m_halfSize.y))
    {
      return true;
    }
    return false;
  }

  inline bool contains(glm::i64vec2 const & p_pos) const
  {
       if(glm::i64(m_pos.x + m_halfSize.x) < p_pos.x
       || glm::i64(m_pos.x - m_halfSize.x) > p_pos.x
       || glm::i64(m_pos.y + m_halfSize.y) < p_pos.y
       || glm::i64(m_pos.y - m_halfSize.y) > p_pos.y)
    {
      return false;
    }
    return true;
  }

  inline bool intersects(AABB const & p_other) const
  {
    // what happens when we have a near world sized AABB that goes out of bounds of the world?
    if(glm::i64(m_pos.x + m_halfSize.x) <= glm::i64(p_other.m_pos.x - p_other.m_halfSize.x)
    || glm::i64(m_pos.x - m_halfSize.x) >= glm::i64(p_other.m_pos.x + p_other.m_halfSize.x)
    || glm::i64(m_pos.y + m_halfSize.y) <= glm::i64(p_other.m_pos.y - p_other.m_halfSize.y)
    || glm::i64(m_pos.y - m_halfSize.y) >= glm::i64(p_other.m_pos.y + p_other.m_halfSize.y))
    {
      return false;
    }
    return true;
  }

  inline void print() const
  {
    std::cout << toString() << std::endl;
  }

  inline std::string toString() const
  {
    std::stringstream strstr;
    strstr << "X:" << m_pos.x << " Y:" << m_pos.y << " W:" << m_halfSize.x * 2 << " H:" << m_halfSize.y * 2;
    return strstr.str();
  }

  glm::i64vec2 m_pos;
  glm::u64vec2 m_halfSize;
};

#endif
