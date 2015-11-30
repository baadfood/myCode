#pragma once

#include <glm/glm.hpp>
#include <vector>

class Object;
struct AABB;

class SpatialTree
{
public:

  virtual bool sanityCheck() = 0;
  virtual SpatialTree * top() = 0;
  virtual SpatialTree * addObject(Object * p_object) = 0;
  virtual void removeObject(Object * p_object) = 0;
  virtual void updateObjectPos(Object * p_object, size_t p_index) = 0;
  virtual Object * getObjectClosestHitObject(glm::i64vec2 const & p_worldPos) = 0;
  virtual void getObjectsHitInPriority(glm::i64vec2 const & p_worldPos, std::vector<Object*> & p_objects) = 0;
  virtual void getObjectsAt(glm::i64vec2 const & p_worldPos, std::vector<Object*> & p_objects) = 0;
  virtual void getObjectsIn(AABB const & p_aabb, std::vector<Object*> & p_objects) = 0;
  virtual void getObjectsIntersected(AABB const & p_aabb, std::vector<Object*> & p_objects) = 0;
};

