#include "MtQuadTree.h"
#include "Object.h"
#include <cassert>
#include "utils.h"
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <boost/lockfree/queue.hpp>

namespace
{
static const float s_fatness = 1.0f;
static boost::lockfree::queue<MtQuadTree*> s_MtQuadTrees(100);
}

MtQuadTree * MtQuadTree::getQuadTree()
{
  MtQuadTree * ret;
  if(s_MtQuadTrees.pop(ret))
  {
    return ret;
  }
  else
  {
    return new MtQuadTree();
  }
}
void MtQuadTree::freeQuadTree(MtQuadTree * p_node)
{
  Object * obj;
  while(p_node->m_objectsToAddToNewParent.pop(obj));
  unsigned int val;
  while(p_node->m_objectsToRemove.pop(val));
  while(p_node->m_objectsToAdd.pop(obj));
  p_node->m_indivisible = false;
  p_node->m_divided = false;
  p_node->m_objects.clear();
  s_MtQuadTrees.push(p_node);
}


MtQuadTree::MtQuadTree(AABB const & p_boundary):
m_parent(nullptr),
m_ne(nullptr),
m_nw(nullptr),
m_se(nullptr),
m_sw(nullptr),
m_boundary(p_boundary),
m_divided(false),
m_objectsToAdd(10),
m_objectsToRemove(10),
m_objectsToAddToNewParent(1)
{
  if(m_boundary.m_halfSize.x < s_atomSize)
  {
    m_indivisible = true;
  }
  m_objects.reserve(s_capacity * 2);
}

MtQuadTree * MtQuadTree::top()
{
  if(m_parent != nullptr)
  {
    return m_parent->top();
  }
  return this;
}

bool MtQuadTree::contains(AABB const & p_aabb) const
{
  return m_boundary.contains(p_aabb);
}

MtQuadTree::~MtQuadTree()
{
  delete m_ne;
  delete m_nw;
  delete m_se;
  delete m_sw;
}

void MtQuadTree::deleteThis()
{
  m_lock.lock();
  if(m_parent)
  {
    m_parent->m_lock.lock();
    m_parent->childDeleted(this);
    m_parent->m_lock.unlock();
    if(m_divided)
    {
      m_ne->deleteThis_Internal();
      m_nw->deleteThis_Internal();
      m_se->deleteThis_Internal();
      m_sw->deleteThis_Internal();
    }
  }
  m_lock.unlock();
  delete this;
}

void MtQuadTree::deleteThis_Internal()
{
  m_lock.lock();
  if(m_parent)
  {
    m_parent->childDeleted(this);
    if(m_divided)
    {
      m_ne->deleteThis_Internal();
      m_nw->deleteThis_Internal();
      m_se->deleteThis_Internal();
      m_sw->deleteThis_Internal();
    }
  }
  m_lock.unlock();
  delete this;
}

void MtQuadTree::childDeleted(MtQuadTree * p_child)
{
  if(m_ne == p_child)
  {
    m_ne = nullptr;
  }
  if(m_nw == p_child)
  {
    m_nw = nullptr;
  }
  if(m_se == p_child)
  {
    m_se = nullptr;
  }
  if(m_sw == p_child)
  {
    m_sw = nullptr;
  }
}


MtQuadTree * MtQuadTree::addObject(Object * p_object)
{
  MtQuadTree * root = top();
  if(addObject_Internal(p_object) == false)
  {
    root->addObject_Internal(p_object);
  }
  return root;

/*
  if(m_parent)
  {
    MtQuadTree * topNode = top();
    return topNode->addObject(p_object);
  }
  // Find the correct top level thing and start from there.
  if(addObject_Internal(p_object) == false)
  {
    m_lock.lock();

    if(m_parent == NULL)
    {
      EDirection direction = m_boundary.getDirectionTo(p_object->getAABB());

      AABB parentAABB(m_boundary);
      parentAABB.m_halfSize *= 2;

      m_parent = getQuadTree();

      switch(direction)
      {
        case eN:
        case eNe:
        parentAABB.setCenter(m_boundary.getEdge(eNe));
        m_parent->m_sw = this;
        break;
        case eE:
        case eSe:
        parentAABB.setCenter(m_boundary.getEdge(eSe));
        m_parent->m_nw = this;
        break;
        case eS:
        case eSw:
        parentAABB.setCenter(m_boundary.getEdge(eSw));
        m_parent->m_ne = this;
        break;
        case eW:
        case eNw:
        parentAABB.setCenter(m_boundary.getEdge(eNw));
        m_parent->m_se = this;
        break;
        default: // It's right in the middle
        parentAABB.setCenter(m_boundary.getEdge(eNe));
        m_parent->m_sw = this;
        break;
      }
      m_parent->m_boundary = parentAABB;
      m_parent->subdivide();
      m_parent->m_objects = m_objects;
    }
    m_lock.unlock();
    m_parent->addObject(p_object);
  }

  return top();*/
}

void MtQuadTree::removeObject(Object * p_object)
{
  if(mika::removeOne(m_objects, p_object) == false)
  {
    assert(false);
    if(m_divided)
    {
      m_ne->removeObject(p_object);
      m_se->removeObject(p_object);
      m_nw->removeObject(p_object);
      m_sw->removeObject(p_object);
    }
  }
  return;
}

bool MtQuadTree::addObject_Internal(Object * p_object)
{
  if(m_boundary.contains(p_object->getAABB()) == false)
  {
    if(m_parent == nullptr)
    {
      m_objectsToAddToNewParent.push(p_object);
      return true;
    }
    return false;
  }
  if(m_divided)
  {
    if(m_ne->addObject_Internal(p_object)
    || m_se->addObject_Internal(p_object)
    || m_sw->addObject_Internal(p_object)
    || m_nw->addObject_Internal(p_object))
    {
      return true;
    }
  }

  m_objectsToAdd.push(p_object);
  return true;
}

bool MtQuadTree::addObject_Internal_Direct(Object * p_object)
{
  if (m_boundary.contains(p_object->getAABB()) == false)
  {
    if (m_parent == nullptr)
    {
      m_objectsToAddToNewParent.push(p_object);
      return true;
    }
    return false;
  }
  if (m_divided)
  {
    if (m_ne->addObject_Internal_Direct(p_object)
      || m_se->addObject_Internal_Direct(p_object)
      || m_sw->addObject_Internal_Direct(p_object)
      || m_nw->addObject_Internal_Direct(p_object))
    {
      return true;
    }
  }

  m_objects.push_back(p_object);
  return true;
}

void MtQuadTree::createNewParent(Object * p_object)
{
  EDirection direction = m_boundary.getDirectionTo(p_object->getAABB());

  AABB parentAABB(m_boundary);
  parentAABB.m_halfSize *= 2;

  m_parent = getQuadTree();

  switch(direction)
  {
    case eN:
    case eNe:
    parentAABB.setCenter(m_boundary.getEdge(eNe));
    m_parent->m_sw = this;
    break;
    case eE:
    case eSe:
    parentAABB.setCenter(m_boundary.getEdge(eSe));
    m_parent->m_nw = this;
    break;
    case eS:
    case eSw:
    parentAABB.setCenter(m_boundary.getEdge(eSw));
    m_parent->m_ne = this;
    break;
    case eW:
    case eNw:
    parentAABB.setCenter(m_boundary.getEdge(eNw));
    m_parent->m_se = this;
    break;
    default: // It's right in the middle
    parentAABB.setCenter(m_boundary.getEdge(eNe));
    m_parent->m_sw = this;
    break;
  }
  m_parent->m_boundary = parentAABB;
}

void MtQuadTree::updateTreeFromChild(MtQuadTree * p_child)
{
  if (m_objectsToAddToNewParent.empty() == false)
  {
    Object * obj;
    while (m_objectsToAddToNewParent.pop(obj))
    {
      if (m_parent == nullptr)
      {
        createNewParent(obj);
      }
      m_parent->addObject(obj);
    }
    m_parent->updateTreeFromChild(this);
    /*    auto task = std::make_unique<std::future<void>>(getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_parent)));
    getThreadPool().waitAndDoTasks();
    while(task->_Is_ready() == false)
    {
    getThreadPool().waitAndDoTasks();
    }*/
  }
  int lastIndex = m_objects.size() - 1;
  unsigned int index;
  std::vector<size_t> toRemove;
  while (m_objectsToRemove.unsynchronized_pop(index))
  {
    toRemove.push_back((size_t)index);
  }
  std::sort(toRemove.begin(), toRemove.end());
  mika::removeIndices(m_objects, toRemove);

  Object * object;
  std::vector<Object * > objectsAlreadyAdded;
  while (m_objectsToAdd.unsynchronized_pop(object))
  {
    m_objects.push_back(object);
    object->setTreeNode(this, m_objects.size() - -1);
  }

  if (m_objects.size() > s_capacity)
  {
    subdivide();
  }

  if (m_divided)
  {

    /*
    m_ne->updateTree();
    m_se->updateTree();
    m_sw->updateTree();
    m_nw->updateTree();

    /*/
    bool neUpdate = m_ne->somethingToUpdate();
    bool seUpdate = m_se->somethingToUpdate();
    bool swUpdate = m_sw->somethingToUpdate();
    bool nwUpdate = m_nw->somethingToUpdate();
    if (neUpdate)
    {
      if (p_child != m_ne)getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_ne));
    }
    if (seUpdate)
    {
      if (p_child != m_se)getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_se));
    }
    if (swUpdate)
    {
      if (p_child != m_sw)getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_sw));
    }
    if (nwUpdate)
    {
      if (p_child != m_nw)getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_nw));
    }

    if (!neUpdate) if (p_child != m_ne)m_ne->updateTree();
    if (!seUpdate) if (p_child != m_se)m_se->updateTree();
    if (!swUpdate) if (p_child != m_sw)m_sw->updateTree();
    if (!nwUpdate) if (p_child != m_nw)m_nw->updateTree();

    /*
    if (p_child != m_ne)
    {
      getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_ne));
    }
    if (p_child != m_se)
    {
      getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_se));
    }
    if (p_child != m_sw)
    {
      getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_sw));
    }
    if (p_child != m_nw)
    {
      getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_nw));
    }
    //*/
    //      tasks.push_back(std::make_unique<std::future<void>>(getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_ne))));
    //      tasks.push_back(std::make_unique<std::future<void>>(getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_se))));
    //      tasks.push_back(std::make_unique<std::future<void>>(getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_sw))));
    //      tasks.push_back(std::make_unique<std::future<void>>(getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_nw))));
  }
}

bool MtQuadTree::somethingToUpdate()
{
  return !(m_objectsToAddToNewParent.empty() && m_objectsToAdd.empty() && m_objectsToRemove.empty());
}

void MtQuadTree::updateTree()
{
  if(m_objectsToAddToNewParent.empty() == false)
  {
    Object * obj;
    while(m_objectsToAddToNewParent.pop(obj))
    {
      if(m_parent == nullptr)
      {
        createNewParent(obj);
      }
      m_parent->addObject(obj);
    }
    m_parent->updateTreeFromChild(this);
/*    auto task = std::make_unique<std::future<void>>(getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_parent)));
    getThreadPool().waitAndDoTasks();
    while(task->_Is_ready() == false)
    {
      getThreadPool().waitAndDoTasks();
    }*/
  }
  int lastIndex = m_objects.size() - 1;
  unsigned int index;
  std::vector<size_t> toRemove;
  while(m_objectsToRemove.unsynchronized_pop(index))
  {
    toRemove.push_back((size_t)index);
  }
  std::sort(toRemove.begin(), toRemove.end());
  mika::removeIndices(m_objects, toRemove);

  Object * object;
  while(m_objectsToAdd.unsynchronized_pop(object))
  {
    m_objects.push_back(object);
    object->setTreeNode(this, m_objects.size() - 1);
  }

  if (m_objects.size() > s_capacity)
  {
    subdivide();
  }

  if (m_divided)
  {

/*
    m_ne->updateTree();
    m_se->updateTree();
    m_sw->updateTree();
    m_nw->updateTree();

    /*/
    bool neUpdate = m_ne->somethingToUpdate();
    bool seUpdate = m_se->somethingToUpdate();
    bool swUpdate = m_sw->somethingToUpdate();
    bool nwUpdate = m_nw->somethingToUpdate();
    if (neUpdate)
    {
      getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_ne));
    }
    if (seUpdate)
    {
      getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_se));
    }
    if (swUpdate)
    {
      getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_sw));
    }
    if (nwUpdate)
    {
      getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_nw));
    }

    if (!neUpdate) m_ne->updateTree();
    if (!seUpdate) m_se->updateTree();
    if (!swUpdate) m_sw->updateTree();
    if (!nwUpdate) m_nw->updateTree();

    /*
    getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_ne));
    getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_se));
    getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_sw));
    getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_nw));
    //*/
    //      tasks.push_back(std::make_unique<std::future<void>>(getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_ne))));
    //      tasks.push_back(std::make_unique<std::future<void>>(getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_se))));
    //      tasks.push_back(std::make_unique<std::future<void>>(getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_sw))));
    //      tasks.push_back(std::make_unique<std::future<void>>(getThreadPool().push(std::bind(&MtQuadTree::updateTree, m_nw))));
  }
}

void MtQuadTree::subdivide()
{
  glm::i64vec2 offset(m_boundary.m_halfSize / static_cast<glm::u64>(2));
  AABB childAabb;
  childAabb.setSize(static_cast<glm::u64vec2>(static_cast<glm::f64vec2>(m_boundary.m_halfSize) / glm::f64(2 / s_fatness)));

  if(m_ne == nullptr)
  {
    childAabb.setCenter(m_boundary.getEdge(eNe) - offset);
    m_ne = getQuadTree();
    m_ne->m_boundary = childAabb;
    m_ne->m_parent = this;
  }

  if(m_sw == nullptr)
  {
    childAabb.setCenter(m_boundary.getEdge(eSw) + offset);
    m_sw = getQuadTree();
    m_sw->m_boundary = childAabb;
    m_sw->m_parent = this;
  }

  glm::i64 x = offset.x;
  offset.x = -offset.y;
  offset.y = x;

  if(m_se == nullptr)
  {
    childAabb.setCenter(m_boundary.getEdge(eSe) + offset);
    m_se = getQuadTree();
    m_se->m_boundary = childAabb;
    m_se->m_parent = this;
  }

  if(m_nw == nullptr)
  {
    childAabb.setCenter(m_boundary.getEdge(eNw) - offset);
    m_nw = getQuadTree();
    m_nw->m_boundary = childAabb;
    m_nw->m_parent = this;
  }

  for (int index = 0;
  index != m_objects.size();
    )
  {
    Object * object = m_objects[index];
    if(m_ne->addObject_Internal_Direct(object)
    || m_se->addObject_Internal_Direct(object)
    || m_nw->addObject_Internal_Direct(object)
    || m_sw->addObject_Internal_Direct(object))
    {
      std::swap(m_objects.back(), m_objects[index]);
      m_objects.pop_back(); // BAD?
      m_objects.back()->setTreeNode(this, m_objects.size() - 1);
    }
    else
    {
      index++;
      object->setTreeNode(this, m_objects.size() - 1);
    }
  }

  m_divided = true;
}

bool MtQuadTree::sanityCheck()
{
  for(auto iter = m_objects.begin();
  iter != m_objects.end();
    iter++)
  {
    if(m_boundary.contains((*iter)->getAABB()) == false)
    {
      std::stringstream strstr;
      if(m_parent)
      {
        m_parent->print(strstr, "");
      }
      else
      {
        print(strstr, "");
      }
      std::cout << "Object in wrong place " << (*iter)->getName() << std::endl;
      std::cout << strstr.str();
      return false;
    }
    else if ((*iter)->getTreeNode() != this)
    {
      std::cout << "Object treenode is wrong" << (*iter)->getName() << std::endl;
      return false;
    }
  }
  if(m_divided)
  {
    if(m_ne->sanityCheck() == false) return false;
    if(m_se->sanityCheck() == false) return false;
    if(m_sw->sanityCheck() == false) return false;
    if(m_nw->sanityCheck() == false) return false;
  }
  return true;
}

bool MtQuadTree::nodeHasObject(Object * p_object)
{
  auto iter = std::find(m_objects.begin(), m_objects.end(), p_object);
  return iter != m_objects.end();
}

void MtQuadTree::updateObjectPos(Object * p_object, size_t p_index)
{
  if(m_boundary.contains(p_object->getAABB()))
  {
    if(m_divided)
    {
      if(m_ne->addObject_Internal(p_object))
      {
        m_objectsToRemove.push(mika::indexOf(m_objects, p_object));
        return;
      }
      if(m_se->addObject_Internal(p_object))
      {
        m_objectsToRemove.push(mika::indexOf(m_objects, p_object));
        return;
      }
      if(m_nw->addObject_Internal(p_object))
      {
        m_objectsToRemove.push(mika::indexOf(m_objects, p_object));
        return;
      }
      if(m_sw->addObject_Internal(p_object))
      {
        m_objectsToRemove.push(mika::indexOf(m_objects, p_object));
        return;
      }
    }
    return;
  }
  if(m_parent == nullptr)
  {
    std::cout << "Not sure about this case....\n";
    return;
  } 

  m_objectsToRemove.push(mika::indexOf(m_objects, p_object));

  MtQuadTree * node = this->m_parent;

  while(node->m_boundary.contains(p_object->getAABB()) == false)
  {
    if(node->m_parent == nullptr)
    {
      node->addObject(p_object);
      return;
    }
    else
    {
      node = node->m_parent;
    }
  }

  if(node->m_divided)
  {
    if(node->m_ne->addObject_Internal(p_object)) return;
    if(node->m_se->addObject_Internal(p_object)) return;
    if(node->m_nw->addObject_Internal(p_object)) return;
    if(node->m_sw->addObject_Internal(p_object)) return;
  }
  node->m_objectsToAdd.push(p_object);
}

Object * MtQuadTree::getObjectClosestHitObject(glm::i64vec2 const & p_worldPos)
{
  if(m_divided)
  {
    if(m_ne->m_boundary.contains(p_worldPos))
    {
      return m_ne->getObjectClosestHitObject(p_worldPos);
    }
    if(m_se->m_boundary.contains(p_worldPos))
    {
      return m_se->getObjectClosestHitObject(p_worldPos);
    }
    if(m_sw->m_boundary.contains(p_worldPos))
    {
      return m_sw->getObjectClosestHitObject(p_worldPos);
    }
    if(m_nw->m_boundary.contains(p_worldPos))
    {
      return m_nw->getObjectClosestHitObject(p_worldPos);
    }
  }
  double distance = std::numeric_limits<double>::max();
  Object * closest = 0;
  double currentDistance;

  for(auto iter = m_objects.begin();
  iter != m_objects.end();
    iter++)
  {
    if((*iter)->getAABB().contains(p_worldPos))
    {
      currentDistance = distanceSquared((*iter)->getAABB().m_pos, p_worldPos);
      if(currentDistance < distance)
      {
        distance = currentDistance;
        closest = (*iter);
      }
    }
  }
  return closest;
}

bool priority(glm::i64vec2 const &  p_worldPos, Object const * p_first, Object const * p_second)
{
  return distanceSquared(p_first->getAABB().m_pos, p_worldPos) < distanceSquared(p_second->getAABB().m_pos, p_worldPos);
}

void MtQuadTree::getObjectsHitInPriority(glm::i64vec2 const & p_worldPos, std::vector<Object*> & p_objects)
{
  p_objects.clear();
  top()->getObjectsHitInPriority_Internal(p_worldPos, p_objects);

  auto sortFunc = std::bind(priority, p_worldPos, std::placeholders::_1, std::placeholders::_2);

  std::sort(p_objects.begin(), p_objects.end(), sortFunc);
}

void MtQuadTree::getObjectsHitInPriority_Internal(glm::i64vec2 const & p_worldPos, std::vector<Object*> & p_objects)
{
  if(m_divided)
  {
    if(m_ne->m_boundary.contains(p_worldPos))
    {
      m_ne->getObjectsHitInPriority_Internal(p_worldPos, p_objects);
    }
    else if(m_se->m_boundary.contains(p_worldPos))
    {
      m_se->getObjectsHitInPriority_Internal(p_worldPos, p_objects);
    }
    else if(m_sw->m_boundary.contains(p_worldPos))
    {
      m_sw->getObjectsHitInPriority_Internal(p_worldPos, p_objects);
    }
    else if(m_nw->m_boundary.contains(p_worldPos))
    {
      m_nw->getObjectsHitInPriority_Internal(p_worldPos, p_objects);
    }
  }

  for(auto iter = m_objects.begin();
  iter != m_objects.end();
    iter++)
  {
    if((*iter)->getAABB().contains(p_worldPos))
    {
      p_objects.push_back((*iter));
    }
  }
}

void MtQuadTree::getObjectsAt(glm::i64vec2 const & p_worldPos, std::vector<Object*> & p_objects)
{
  p_objects.clear();
  top()->getObjectsHitInPriority_Internal(p_worldPos, p_objects);
}

void MtQuadTree::getObjectsIn(AABB const & p_aabb, std::vector<Object*> & p_objects)
{
  if(m_parent)
  {
    return top()->getObjectsIn(p_aabb, p_objects);
  }
  p_objects.clear();
  if(m_boundary.contains(p_aabb) == false)
  {
    for(auto iter = m_objects.begin();
    iter != m_objects.end();
      iter++)
    {
      if(p_aabb.contains((*iter)->getAABB()))
      {
        p_objects.push_back(*iter);
      }
    }
    return;
  }
  else
  {
    return getObjectsIn_Internal(p_aabb, p_objects);
  }
}

void MtQuadTree::getObjectsIn_Internal(AABB const & p_aabb, std::vector<Object*> & p_objects)
{
  if(m_divided)
  {
    if(m_ne->m_boundary.intersects(p_aabb))
    {
      m_ne->getObjectsIn_Internal(p_aabb, p_objects);
    }
    if(m_se->m_boundary.intersects(p_aabb))
    {
      m_se->getObjectsIn_Internal(p_aabb, p_objects);
    }
    if(m_sw->m_boundary.intersects(p_aabb))
    {
      m_sw->getObjectsIn_Internal(p_aabb, p_objects);
    }
    if(m_nw->m_boundary.intersects(p_aabb))
    {
      m_nw->getObjectsIn_Internal(p_aabb, p_objects);
    }
  }
  for(auto iter = m_objects.begin();
      iter != m_objects.end();
      iter++)
  {
    if(p_aabb.contains((*iter)->getAABB()))
    {
      p_objects.push_back(*iter);
    }
  }
}

void MtQuadTree::getObjectsIntersected(AABB const & p_aabb, std::vector<Object*> & p_objects)
{
  if(m_parent)
  {
    return top()->getObjectsIntersected(p_aabb, p_objects);
  }
  p_objects.clear();
  if(m_boundary.contains(p_aabb) == false)
  {
    for(auto iter = m_objects.begin();
    iter != m_objects.end();
      iter++)
    {
      if(p_aabb.intersects((*iter)->getAABB()))
      {
        p_objects.push_back(*iter);
      }
    }
    return;
  }
  else
  {
    return getObjectsIntersected_Internal(p_aabb, p_objects);
  }
}

void MtQuadTree::getObjectsIntersected_Internal(AABB const & p_aabb, std::vector<Object*> & p_objects)
{
  if(m_divided)
  {
    if(m_ne->m_boundary.intersects(p_aabb))
    {
      m_ne->getObjectsIntersected_Internal(p_aabb, p_objects);
    }
    if(m_se->m_boundary.intersects(p_aabb))
    {
      m_se->getObjectsIntersected_Internal(p_aabb, p_objects);
    }
    if(m_sw->m_boundary.intersects(p_aabb))
    {
      m_sw->getObjectsIntersected_Internal(p_aabb, p_objects);
    }
    if(m_nw->m_boundary.intersects(p_aabb))
    {
      m_nw->getObjectsIntersected_Internal(p_aabb, p_objects);
    }
  }
  for(auto iter = m_objects.begin();
  iter != m_objects.end();
    iter++)
  {
    if(p_aabb.intersects((*iter)->getAABB()))
    {
      p_objects.push_back(*iter);
    }
  }
}


void MtQuadTree::pruneTree()
{
  if(m_objects.size() != 0
     || m_divided == false)
  {
    return;
  }
  m_divided = false;
  m_ne->deleteThis();
  m_nw->deleteThis();
  m_se->deleteThis();
  m_sw->deleteThis();
}

void MtQuadTree::print(std::ostream & p_stream, std::string const & p_indentation)
{
  p_stream << p_indentation << "MtQuadTree " << std::endl;
  p_stream << p_indentation << "AABB " << m_boundary.toString() << std::endl;
  p_stream << p_indentation << "Object count " << m_objects.size() << std::endl;
  p_stream << p_indentation << "Object names ";
  for(auto iter = m_objects.begin();
  iter != m_objects.end();
    iter++)
  {
    p_stream << (*iter)->getName() << ", ";
  }
  p_stream << std::endl;

  if(m_divided)
  {
    m_ne->print(p_stream, std::string(p_indentation) += ("NE-"));
    m_se->print(p_stream, std::string(p_indentation) += ("SE-"));
    m_sw->print(p_stream, std::string(p_indentation) += ("SW-"));
    m_nw->print(p_stream, std::string(p_indentation) += ("NW-"));
  }
}
