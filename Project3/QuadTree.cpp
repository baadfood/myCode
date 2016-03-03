#include <glm/glm.hpp>
#include "QuadTree.h"
#include "Object.h"
#include <cassert>
#include "utils.h"
#include <map>
#include <iostream>
#include <string>
#include <sstream>

namespace
{
  std::vector<QuadTree*> s_freeNodes;
  static void freeNode(QuadTree * p_node)
  {
    s_freeNodes.push_back(p_node);
  }
  static QuadTree * getNode(AABB const & p_boundary)
  {
    QuadTree * retval;
    if (s_freeNodes.empty())
    {
      retval = new QuadTree(p_boundary);
    }
    else
    {
      retval = s_freeNodes.back();
      s_freeNodes.pop_back();
      retval->setBoundary(p_boundary);
    }
    retval->reset();
    return retval;
  }
}

void QuadTree::setBoundary(AABB const & p_boundary)
{
  m_boundary = p_boundary;
  if (m_boundary.m_halfSize.x < s_atomSize)
  {
    m_indivisible = true;
  }
  else
  {
    m_indivisible = false;
  }
}

void QuadTree::reset()
{
  m_objects.clear();
  m_parent = nullptr;
  m_ne = nullptr;
  m_nw = nullptr;
  m_se = nullptr;
  m_sw = nullptr;
  m_divided = false;
}

QuadTree::QuadTree(AABB const & p_boundary):
m_parent(nullptr),
m_ne(nullptr),
m_nw(nullptr),
m_se(nullptr),
m_sw(nullptr),
m_boundary(p_boundary),
m_divided(false)
{
  if(m_boundary.m_halfSize.x < s_atomSize)
  {
    m_indivisible = true;
  }
//  m_objects.reserve(s_capacity * 2);
}

QuadTree * QuadTree::top()
{
  if(m_parent != nullptr)
  {
    return m_parent->top();
  }
  return this;
}


QuadTree::~QuadTree()
{
  delete m_ne;
  delete m_nw;
  delete m_se;
  delete m_sw;
}

void QuadTree::deleteThis()
{
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
  delete this;
}

void QuadTree::deleteThis_Internal()
{
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
  delete this;
}

void QuadTree::childDeleted(QuadTree * p_child)
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

QuadTree * QuadTree::addObject(Object * p_object)
{
  if(m_parent)
  {
    QuadTree * topNode = top();
    return topNode->addObject(p_object);
  }
  // Find the correct top level thing and start from there.
  if(addObject_Internal(p_object) == false)
  {
    if(m_parent == NULL)
    {
      EDirection direction = m_boundary.getDirectionTo(p_object->getAABB());

      AABB parentAABB(m_boundary);
      parentAABB.m_halfSize *= 2;
      assert(m_boundary.m_halfSize.x != 0);

      switch(direction)
      {
        case eN:
        case eNe:
        parentAABB.setCenter(m_boundary.getEdge(eNe));
        m_parent = getNode(parentAABB);
        m_parent->m_sw = this;
        break;
        case eE:
        case eSe:
        parentAABB.setCenter(m_boundary.getEdge(eSe));
        m_parent = getNode(parentAABB);
        m_parent->m_nw = this;
        break;
        case eS:
        case eSw:
        parentAABB.setCenter(m_boundary.getEdge(eSw));
        m_parent = getNode(parentAABB);
        m_parent->m_ne = this;
        break;
        case eW:
        case eNw:
        parentAABB.setCenter(m_boundary.getEdge(eNw));
        m_parent = getNode(parentAABB);
        m_parent->m_se = this;
        break;
        default: // It's right in the middle
        parentAABB.setCenter(m_boundary.getEdge(eNe));
        m_parent = getNode(parentAABB);
        m_parent->m_sw = this;
        break;
      }
      m_parent->subdivide();
    }
    m_parent->addObject(p_object);
  }

  return top();
}

void QuadTree::removeObject(Object * p_object)
{
/*  if(mika::removeOne(m_objects, p_object))
  {
    if(m_divided)
    {
      m_ne->removeObject(p_object);
      m_se->removeObject(p_object);
      m_nw->removeObject(p_object);
      m_sw->removeObject(p_object);
    }
  }*/
  return;
}

bool QuadTree::addObject_Internal(Object * p_object)
{
  if(m_boundary.contains(p_object->getAABB()) == false)
  {
    return false;
  }
/*
  if(nodeHasObject(p_object))
  {
    std::cerr << "Node already had Object\n";
    assert(false);
  }
*/
  if(m_divided == false
  ||  (m_ne->addObject_Internal(p_object) == false
    && m_se->addObject_Internal(p_object) == false
    && m_sw->addObject_Internal(p_object) == false
    && m_nw->addObject_Internal(p_object) == false) )
  {
    m_objects.push_back(p_object);
    p_object->setTreeNode(this, m_objects.size() - 1);
  }
  if(m_objects.size() > s_capacity
  && m_divided == false
  && m_indivisible == false)
  {
    subdivide();
  }
  return true;
}

void QuadTree::subdivide()
{
  glm::i64vec2 offset(static_cast<glm::i64vec2>(m_boundary.m_halfSize / static_cast<glm::u64>(2)));
  AABB childAabb;
  childAabb.setSize(m_boundary.m_halfSize / static_cast<glm::u64>(2));

  if(m_ne == nullptr)
  {
    childAabb.setCenter(m_boundary.getEdge(eNe) - offset);
    m_ne = getNode(childAabb);
    m_ne->m_parent = this;
  }
 
  if(m_sw == nullptr)
  {
    childAabb.setCenter(m_boundary.getEdge(eSw) + offset);
    m_sw = getNode(childAabb);
    m_sw->m_parent = this;
  }

  glm::i64 x = offset.x;
  offset.x = -offset.y;
  offset.y = x;

  if(m_se == nullptr)
  {
    childAabb.setCenter(m_boundary.getEdge(eSe) + offset);
    m_se = getNode(childAabb);
    m_se->m_parent = this;
  }

  if(m_nw == nullptr)
  {
    childAabb.setCenter(m_boundary.getEdge(eNw) - offset);
    m_nw = getNode(childAabb);
    m_nw->m_parent = this;
  }

  for(int index = 0;
  index != m_objects.size();
    )
  {
    Object * object = m_objects[index];
    if(m_ne->addObject_Internal(object)
    || m_se->addObject_Internal(object)
    || m_nw->addObject_Internal(object)
    || m_sw->addObject_Internal(object))
    {
      if (m_objects.size() == index+1)
      {
        m_objects.pop_back();
      }
      else
      {
        std::swap(m_objects.back(), m_objects[index]);
        m_objects.pop_back();
        if (m_objects.empty() == false)
        {
          m_objects[index]->setTreeNode(this, index);
        }
      }
    }
    else
    {
      object->setTreeNode(this, index);
      index++;
    }
  }

  m_divided = true;
}

bool QuadTree::sanityCheck()
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

bool QuadTree::nodeHasObject(Object * p_object)
{
  auto iter = std::find(m_objects.begin(), m_objects.end(), p_object);
  return iter != m_objects.end();
}

void QuadTree::updateObjectPos(Object * p_object, size_t p_index)
{
  QuadTree * node = this;
  assert(m_objects[p_index] == p_object);
/*  if(mika::removeOne(m_objects, p_object) == false)
  {
    // WHERE IS IT?
    assert(false);
  }
  */
  if (m_objects.size() == p_index+1)
  {
    m_objects.pop_back();
  }
  else
  {
    std::swap(m_objects.back(), m_objects[p_index]);
    m_objects.pop_back();
    m_objects[p_index]->setTreeNode(this, p_index);
  }
  while(node->addObject_Internal(p_object) == false)
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
/*
  if(node->m_divided)
  {
    if(node->m_ne->addObject_Internal(p_object)) return;
    if(node->m_se->addObject_Internal(p_object)) return;
    if(node->m_nw->addObject_Internal(p_object)) return;
    if(node->m_sw->addObject_Internal(p_object)) return;
  }
  node->m_objects.push_back(p_object);
  p_object->setTreeNode(node, node->m_objects.size() - 1);
*/
  pruneTree();
}

Object * QuadTree::getObjectClosestHitObject(glm::i64vec2 const & p_worldPos)
{
  std::vector<Object*> p_objects;
  top()->getObjectsHit_Internal(p_worldPos, p_objects);
  double distance = std::numeric_limits<double>::max();
  Object * closest = 0;
  double currentDistance;

  for(auto iter = p_objects.begin();
      iter != p_objects.end();
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

namespace
{
bool priority(glm::i64vec2 p_worldPos, Object const * p_first, Object const * p_second)
{
  return distanceSquared(p_first->getAABB().m_pos, p_worldPos) < distanceSquared(p_second->getAABB().m_pos, p_worldPos);
}
}

void QuadTree::getObjectsHitInPriority(glm::i64vec2 const & p_worldPos, std::vector<Object*> & p_objects)
{
  p_objects.clear();
  top()->getObjectsHit_Internal(p_worldPos, p_objects);

  auto sortFunc = std::bind(priority, p_worldPos, std::placeholders::_1, std::placeholders::_2);

  std::sort(p_objects.begin(), p_objects.end(), sortFunc);
}

void QuadTree::getObjectsHit_Internal(glm::i64vec2 const & p_worldPos, std::vector<Object*> & p_objects)
{
  if(m_divided)
  {
    if(m_ne->m_boundary.contains(p_worldPos))
    {
      m_ne->getObjectsHit_Internal(p_worldPos, p_objects);
    }
    else if(m_se->m_boundary.contains(p_worldPos))
    {
      m_se->getObjectsHit_Internal(p_worldPos, p_objects);
    }
    else if(m_sw->m_boundary.contains(p_worldPos))
    {
      m_sw->getObjectsHit_Internal(p_worldPos, p_objects);
    }
    else if(m_nw->m_boundary.contains(p_worldPos))
    {
      m_nw->getObjectsHit_Internal(p_worldPos, p_objects);
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

void QuadTree::getObjectsAt(glm::i64vec2 const & p_worldPos, std::vector<Object*> & p_objects)
{
  p_objects.clear();
  top()->getObjectsHit_Internal(p_worldPos, p_objects);
}

void QuadTree::getObjectsIn(AABB const & p_aabb, std::vector<Object*> & p_objects)
{
  if(m_parent)
  {
    return top()->getObjectsIn(p_aabb, p_objects);
  }
  p_objects.clear();
  return getObjectsIn_Internal(p_aabb, p_objects);
}

void QuadTree::getObjectsIn_Internal(AABB const & p_aabb, std::vector<Object*> & p_objects)
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

void QuadTree::getObjectsIntersected(AABB const & p_aabb, std::vector<Object*> & p_objects)
{
  p_objects.clear();
  QuadTree * node = this;
  while (node->m_parent)
  {
    if (node->m_boundary.contains(p_aabb))
    {
      break;
    }
    node = node->m_parent;
  }

  node->getObjectsIntersected_Internal(p_aabb, p_objects);

  while(node->m_parent)
  {
    node = node->m_parent;
    for (auto iter = node->m_objects.begin();
    iter != node->m_objects.end();
      iter++)
    {
      if (p_aabb.intersects((*iter)->getAABB()))
      {
        p_objects.push_back(*iter);
      }
    }
  }
//  std::cout << "Nodes checked: " << nodesChecked << " Objects checked " << objectsChecked << std::endl;
}

void QuadTree::getObjectsIntersected_Internal(AABB const & p_aabb, std::vector<Object*> & p_objects)
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

void QuadTree::pruneTree()
{
  if (m_divided)
  {
    if(m_ne->m_divided == false && m_ne->m_objects.empty()
    && m_se->m_divided == false && m_se->m_objects.empty()
    && m_sw->m_divided == false && m_sw->m_objects.empty()
    && m_nw->m_divided == false && m_nw->m_objects.empty())
    {
      // Undivide.
      freeNode(m_ne);
      freeNode(m_se);
      freeNode(m_sw);
      freeNode(m_nw);
      m_ne = nullptr;
      m_se = nullptr;
      m_sw = nullptr;
      m_nw = nullptr;
      m_divided = false;
    }
  }
  else if(m_objects.empty())
  {
    if (m_parent)
    {
      m_parent->pruneTree();
    }
  }
}

bool QuadTree::contains(AABB const & p_aabb) const
{
  return m_boundary.contains(p_aabb);
}

void QuadTree::print(std::ostream & p_stream, std::string const & p_indentation)
{
  p_stream << p_indentation << "QuadTree " << std::endl;
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
