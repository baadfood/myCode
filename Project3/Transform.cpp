#include "Transform.h"

struct Transform::Private
{
  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale;
};

Transform::Transform(glm::vec3 const & p_pos, glm::vec3 const & p_rot, glm::vec3 const & p_scale)
{
  d = new Private();
  d->position = p_pos;
  d->rotation = p_rot;
  d->scale = p_scale;
}

Transform::~Transform()
{
}

glm::mat4 Transform::getModel() const
{
  glm::mat4 posMatrix = glm::translate(d->position);
  glm::mat4 rotXMatrix = glm::rotate(d->rotation.x, glm::vec3(1.0, 0.0, 0.0));
  glm::mat4 rotYMatrix = glm::rotate(d->rotation.y, glm::vec3(0.0, 1.0, 0.0));
  glm::mat4 rotZMatrix = glm::rotate(d->rotation.z, glm::vec3(0.0, 0.0, 1.0));
  glm::mat4 scaleMatrix = glm::scale(d->scale);

  glm::mat4 rotMatrix = rotZMatrix * rotYMatrix * rotXMatrix;

  return posMatrix * rotMatrix * scaleMatrix;
}


glm::vec3 const & Transform::getPos() const
{
  return d->position;
}

glm::vec3 & Transform::getPos()
{
  return d->position;
}

glm::vec3 const & Transform::getRot() const
{
  return d->rotation;
}

glm::vec3 & Transform::getRot()
{
  return d->rotation;
}

glm::vec3 const & Transform::getScale() const
{
  return d->scale;
}

glm::vec3 & Transform::getScale()
{
  return d->scale;
}

