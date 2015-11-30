#pragma once
#include "SpatialTree.h"

#include "Spinlock.h"
#include "AABB.h"
#include <boost/lockfree/queue.hpp>

class Object;
class MtQuadTree:
  public SpatialTree
{
  static const int s_capacity = 8;
  static const int s_atomSize = 10;

  static MtQuadTree * getQuadTree();
  static void freeQuadTree(MtQuadTree * p_node);

public:
  MtQuadTree(AABB const & p_boundary = AABB());

  void deleteThis();

  virtual MtQuadTree * addObject(Object * p_object);
  virtual void removeObject(Object * p_object);
  
  virtual void updateTree();
  virtual void updateTreeFromChild(MtQuadTree * p_child);

  virtual Object * getObjectClosestHitObject(glm::i64vec2 const & p_worldPos);
  virtual void getObjectsHitInPriority(glm::i64vec2 const & p_worldPos, std::vector<Object*> & p_objects);
  virtual void getObjectsAt(glm::i64vec2 const & p_worldPos, std::vector<Object*> & p_objects);
  virtual void getObjectsIn(AABB const & p_aabb, std::vector<Object*> & p_objects);
  virtual void getObjectsIntersected(AABB const & p_aabb, std::vector<Object*> & p_objects);
  void subdivide();
  void updateObjectPos(Object * p_object, size_t p_index);
  bool sanityCheck();

  void createNewParent(Object * p_object);

  bool somethingToUpdate();


  MtQuadTree * top();

  void pruneTree();

  void print(std::ostream & p_stream, std::string const & p_indentation);

protected:

  void deleteThis_Internal();
  void childDeleted(MtQuadTree * p_child);
  ~MtQuadTree();

  bool nodeHasObject(Object * p_object);
  virtual bool addObject_Internal(Object * p_object);
  virtual bool addObject_Internal_Direct(Object * p_object);
  virtual void getObjectsIn_Internal(AABB const & p_aabb, std::vector<Object*> & p_objects);
  virtual void getObjectsIntersected_Internal(AABB const & p_aabb, std::vector<Object*> & p_objects);
  virtual void getObjectsHitInPriority_Internal(glm::i64vec2 const & p_worldPos, std::vector<Object*> & p_objects);


  AABB m_boundary;
  bool m_divided;
  bool m_indivisible;

  MtQuadTree * m_parent;
  MtQuadTree * m_nw;
  MtQuadTree * m_ne;
  MtQuadTree * m_se;
  MtQuadTree * m_sw;

  std::vector<Object *> m_objects;
  boost::lockfree::queue<Object*> m_objectsToAdd;
  boost::lockfree::queue<Object*> m_objectsToAddToNewParent;
  boost::lockfree::queue<unsigned int> m_objectsToRemove;

  Spinlock m_lock;
};

