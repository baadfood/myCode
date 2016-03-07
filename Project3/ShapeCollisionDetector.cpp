#include "ShapeCollisionDetector.h"

#include "Object.h"
#include "Physics/Shape.h"
#include "Physics/Fixture.h"
#include <vector>

ShapeCollisionDetector::ShapeCollisionDetector()
{
}


ShapeCollisionDetector::~ShapeCollisionDetector()
{
}

bool ShapeCollisionDetector::getCollisions(Object * p_object1, Object * p_object2, std::vector<Contact*> & p_results)
{
  std::vector<Fixture*> const & fixtures1 = p_object1->getFixtures();
  std::vector<Fixture*> const & fixtures2 = p_object2->getFixtures();

  // Don't make objects with many many fixtures.

  bool retval = false;

  for (Fixture * fix1 : fixtures1)
  {
    for (Fixture * fix2 : fixtures2)
    {
      if(fix1->shape->getAabb().intersects(fix2->shape->getAabb()))
      {
        Contact *newContact = nullptr;
        if (m_handlers[fix1->shape->getType()][fix2->shape->getType()]->Collide(newContact, *fix1, p_object1->getTransform2d(), *fix2, p_object2->getTransform2d()))
        {
          p_object1->addContact(newContact);
          p_results.push_back(newContact);
          retval = true;
        }
      }
    }
  }
  return retval;
}

void ShapeCollisionDetector::registerCollisionHandler(int p_type1, int p_type2, Collision* p_handler)
{
  m_handlers[p_type1][p_type2] = p_handler;
}
