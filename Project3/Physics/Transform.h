#ifndef PhysTransform2d_h_
#define PhysTransform2d_h_

#include <glm/glm.hpp>

struct Transform2d
{
  Transform2d(glm::i64vec2 p_pos = glm::i64vec2(0,0), glm::fvec2 p_rot = glm::fvec2(0,0)) :
  pos(p_pos),
  rot(p_rot)
  { }
  glm::i64vec2 pos;
  glm::fvec2 rot;

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

};


#endif