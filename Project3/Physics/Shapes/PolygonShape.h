#ifndef PolygonShape_h_
#define PolygonShape_h_

#include "../Shape.h"
#include "../Transform.h"
#include <glm/glm.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include "utils.h"

#include <assert.h>
#include <vector>

class PolygonShape : public Shape
{
public:

  static const int c_maxVertexCount = 20;

  PolygonShape()
  {
    Shape::m_type = Shape::ePolygon;
  }

  virtual ~PolygonShape()
  {
  }

  virtual void computeAabb(AABB & p_aabb, Transform2d const & p_transform)
  {
    p_aabb.setCenter(p_transform.pos + p_transform.applyRotation(m_pos));
    p_aabb.setSize(glm::u64vec2(m_radius * 1.05, m_radius * 1.05));
  }
  virtual void calculateMassData(MassData & p_massData, glm::float32 p_density)
  {
    /*
    glm::dvec2 dpos = static_cast<glm::dvec2>(pos);
    p_massData.mass = p_density * M_PI * radius * radius;
    p_massData.center = pos;
    p_massData.inertia = p_massData.mass * (0.5f * radius * radius + glm::dot(dpos, dpos));
    */
    glm::f64vec2 centroid(0, 0);
    glm::f64 area = 0;
    glm::f64 I = 0;
    const glm::f64 inv3 = 1.0 / 3.0;
    const glm::f64 inv12 = 1.0 / 12.0;

    for (unsigned int index = 0;
    index < vertexCount;
      index++)
    {
      glm::f64vec2 point1 = vertices[index];
      unsigned int nextIndex = index + 1 < vertexCount ? index + 1 : 0;
      glm::f64vec2 point2 = vertices[nextIndex];

      glm::f64 D = mika::cross(point1, point2);
      glm::f64 triangleArea = 0.5f * D;
      area += triangleArea;

      centroid += triangleArea * inv3 * (point1 + point2);

      glm::f64 intxSqr = point1.x * point1.x + point1.x * point2.x + point2.x * point2.x;
      glm::f64 intySqr = point1.y * point1.y + point1.y * point2.y + point2.y * point2.y;

      I += inv12 * D * (intxSqr + intySqr);
    }
    centroid *= 1.0 / area;

    p_massData.mass = p_density * area;
    p_massData.center = centroid;
    p_massData.inertia = I * p_density;
  }
  virtual bool TestPoint(const Transform2d& p_transform, const glm::i64vec2 & p_pos) const
  {
    glm::dvec2 distance = static_cast<glm::dvec2>(p_pos - p_transform.pos + p_transform.applyRotation(m_pos));
    return glm::dot(distance, distance) <= m_radius * m_radius;
  }
  virtual bool setVertices(std::vector<glm::f64vec2> const & p_vertices)
  {
    if (p_vertices.size() < 2
      || p_vertices.size() > c_maxVertexCount)
    {
      assert(false);
      return false;
    }

    std::vector<glm::f64vec2>::const_iterator rightBottom = p_vertices.cbegin();

    glm::f64 distanceMax = 0;

    for (auto iter = p_vertices.begin();
      iter != p_vertices.end();
      iter++)
    {
      glm::f64 distance = iter->x * iter->x + iter->y * iter->y;
      if (distance > distanceMax)
      {
        distanceMax = distance;
      }

      if (rightBottom->x < iter->x)
      {
        rightBottom = iter;
      }
      else if (rightBottom->x == iter->x)
      {
        if (rightBottom->y > iter->y)
        {
          rightBottom = iter;
        }
      }
    }

    m_radius = std::sqrt(distanceMax);
    unsigned int hull[c_maxVertexCount];
    unsigned int outIndex = 0;
    const unsigned int start = rightBottom - p_vertices.cbegin();
    unsigned int currentIndex = start;

    while (true)
    {
      hull[outIndex] = currentIndex;

      unsigned int nextIndex = 0;
      for (unsigned int index = 1;
      index < p_vertices.size();
        index++)
      {
        if (nextIndex == currentIndex)
        {
          nextIndex = index;
          continue;
        }

        glm::f64vec2 vert1 = p_vertices[nextIndex] - p_vertices[hull[outIndex]];
        glm::f64vec2 vert2 = p_vertices[index] - p_vertices[hull[outIndex]];

        glm::f64 cross = mika::cross(vert1, vert2);
        if (cross < 0)
        {
          nextIndex = index;
        }

        if (cross == 0.0 && vert1.x * vert1.y > vert2.x * vert2.y)
        {
          nextIndex = index;
        }
      }

      ++outIndex;
      currentIndex = nextIndex;

      if (nextIndex == start)
      {
        // looped around
        vertexCount = outIndex;
        break;
      }
    }

    for (unsigned int index = 0;
    index < vertexCount;
      index++)
    {
      vertices[index] = p_vertices[hull[index]];
    }

    for (unsigned int index1 = 0;
      index1 < vertexCount;
      index1++)
    {
      unsigned int index2 = index1 + 1;
      if (index2 == vertexCount) index2 = 0;

      glm::f64vec2 face = vertices[index2] - vertices[index1];

      normals[index1] = glm::normalize(glm::f64vec2(face.y, -face.x));
    }
    return true;
  }

  glm::f64vec2 getSupport(glm::f64vec2 const & p_direction)
  {
    glm::f64 bestProjection = -std::numeric_limits<glm::f64>::max();
    glm::f64vec2 bestVertex;

    for (unsigned int index = 0;
    index < vertexCount;
      index++)
    {
      glm::f64vec2 vertex = vertices[index];
      double projection = glm::dot(vertex, p_direction);

      if (projection > bestProjection)
      {
        bestVertex = vertex;
        bestProjection = projection;
      }
    }
    return bestVertex;
  }

  uint32_t vertexCount;
  glm::f64vec2 vertices[c_maxVertexCount];
  glm::f64vec2 normals[c_maxVertexCount];
};

#endif