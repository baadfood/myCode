#include "Ai.h"

#include <Object.h>

struct Ai::Private
{
  Object * object;
};

Ai::Ai():
d(new Private())
{

}


Ai::~Ai()
{

}

Object* Ai::getObject()
{
  return d->object;
}

void Ai::setObject(Object* p_object)
{
  d->object = p_object;
}

void Ai::updateLogic(glm::i64 p_nanos)
{

}

