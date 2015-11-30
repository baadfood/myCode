#pragma once
#include "SpatialTree.h"

#include "Spinlock.h"
#include "AABB.h"

class QuadTree:
  public SpatialTree
{
  static const int s_capacity = 8;
  static const int s_atomSize = 10;

public:
  QuadTree(AABB const & p_boundary);

  void deleteThis();


  virtual QuadTree * addObject(Object * p_object);
  virtual void removeObject(Object * p_object);
  virtual Object * getObjectClosestHitObject(glm::i64vec2 const & p_worldPos);
  virtual void getObjectsHitInPriority(glm::i64vec2 const & p_worldPos, std::vector<Object*> & p_objects);
  virtual void getObjectsAt(glm::i64vec2 const & p_worldPos, std::vector<Object*> & p_objects);
  virtual void getObjectsIn(AABB const & p_aabb, std::vector<Object*> & p_objects);
  virtual void getObjectsIntersected(AABB const & p_aabb, std::vector<Object*> & p_objects);
  void subdivide();
  void updateObjectPos(Object * p_object, size_t p_index);
  bool sanityCheck();

  QuadTree * top();

  void pruneTree();
  void reset();

  void print(std::ostream & p_stream, std::string const & p_indentation);

  void setBoundary(AABB const & p_aabb); // DO NOT USE UNLESS YOU ARE CRAZY

protected:

  void deleteThis_Internal();
  void childDeleted(QuadTree * p_child);
  ~QuadTree();

  bool nodeHasObject(Object * p_object);
  virtual bool addObject_Internal(Object * p_object);
  virtual void getObjectsIn_Internal(AABB const & p_aabb, std::vector<Object*> & p_objects);
  virtual void getObjectsIntersected_Internal(AABB const & p_aabb, std::vector<Object*> & p_objects);
  virtual void getObjectsHit_Internal(glm::i64vec2 const & p_worldPos, std::vector<Object*> & p_objects);


  AABB m_boundary;
  bool m_divided;
  bool m_indivisible;

  QuadTree * m_parent;
  QuadTree * m_nw;
  QuadTree * m_ne;
  QuadTree * m_se;
  QuadTree * m_sw;

  std::vector<Object *> m_objects;
};

