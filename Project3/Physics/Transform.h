#ifndef PhysTransform2d_h_
#define PhysTransform2d_h_

#include <glm/glm.hpp>
#include <boost/concept_check.hpp>
#include <iostream>

struct Transform2d
{
  Transform2d(glm::i64vec2 p_pos = glm::i64vec2(0,0), glm::fvec2 p_rot = glm::fvec2(1,0)) :
  pos(p_pos),
  rot(p_rot)
  { }
  glm::i64vec2 pos;
  glm::fvec2 rot;
  
  void setRotation(glm::float32 p_rot)
  {
      rot = glm::fvec2(cos(p_rot), sin(p_rot));
  }

  template <class T>
  inline T applyMove(T const & p_pos) const
  {
    return T(p_pos.x + pos.x
           , p_pos.y + pos.y);
  }

  template <class T>
  inline T revertMove(T const & p_pos) const
  {
    return T(p_pos.x - pos.x
           , p_pos.y - pos.y);
  }

  template <class T>
  inline T multiply(T const & p_pos) const
  {
    return T((rot.x * p_pos.x - rot.y * p_pos.y) + pos.x
           , (rot.y * p_pos.x + rot.x * p_pos.y) + pos.y);
  }

  template <class T>
  inline T revertMultiply(T const & p_pos) const
  {
    return T((rot.x * (p_pos.x - pos.x) + rot.y * (p_pos.y - pos.y))
          , (-rot.y * (p_pos.x - pos.x) + rot.x * (p_pos.y - pos.y)));
  }

  template <class T>
  inline T applyRotation(T const & p_pos) const
  {
    return T((rot.x * p_pos.x - rot.y * p_pos.y)
           , (rot.y * p_pos.x + rot.x * p_pos.y));
  }

  template <class T>
  inline T revertRotation(T const & p_pos) const
  {
    return T((rot.x * p_pos.x + rot.y * p_pos.y)
          , (-rot.y * p_pos.x + rot.x * p_pos.y));
  }
  
  inline Transform2d & operator*=(Transform2d const & p_rhs)
  {
    glm::i64vec2 temp;
    
    temp.x = static_cast<int64_t>((p_rhs.rot.x * pos.x - p_rhs.rot.y * pos.y) + p_rhs.pos.x);
    temp.y = static_cast<int64_t>((p_rhs.rot.y * pos.x + p_rhs.rot.x * pos.y) + p_rhs.pos.y);
    
    pos = temp;
    
    glm::f32vec2 rotTemp;
    
    rotTemp.x = p_rhs.rot.x * rot.x - rot.y * p_rhs.rot.y;
    rotTemp.y = p_rhs.rot.y * rot.x + rot.y * p_rhs.rot.x;
    rot = rotTemp;

    return *this;
  }
  
  inline Transform2d operator*(Transform2d const & p_rhs) const
  {
    return Transform2d(*this) *= p_rhs;
  }
  
  inline void print() const
  {
    std::cout << "Pos : " << pos.x << ", " << pos.y << std::endl;
    std::cout << "Rot : " << rot.x << ", " << rot.y << std::endl;
    std::cout << "Angle : " << atan2(pos.y, pos.x) << std::endl;
  }

};


#endif