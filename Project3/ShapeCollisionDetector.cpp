#include "ShapeCollisionDetector.h"

#include "Components/Component.h"
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
  bool retval = false;

  std::vector<Component *> const & components1 = p_object1->getComponents();
  std::vector<Component *> const & components2 = p_object2->getComponents();
  
  for(Component * comp1 : components1)
  {
    for(Component * comp2 : components2)
    {
      std::vector<Fixture * > const & fixtures1 = comp1->getFixtures();
      std::vector<Fixture * > const & fixtures2 = comp2->getFixtures();

      for (Fixture * fix1 : fixtures1)
      {
        for (Fixture * fix2 : fixtures2)
        {
          if(Fixture::fixturesInteract(*fix1, *fix2))
          {
            if(fix1->shape->getAabb().intersects(fix2->shape->getAabb()))
            {
              Transform2d trans1 = comp1->getTransform2d() * p_object1->getTransform2d();
              Transform2d trans2 = comp2->getTransform2d() * p_object2->getTransform2d();
              Contact *newContact = nullptr;
              if (m_handlers[fix1->shape->getType()][fix2->shape->getType()]->Collide(newContact, *fix1, trans1, *fix2, trans2))
              {
                newContact->components[0] = comp1;
                newContact->components[1] = comp2;
                comp1->addContact(newContact);
                newContact->physical = Fixture::fixturesCollide(*fix1, *fix2);
                p_object1->addContact(newContact);
                p_results.push_back(newContact);
                retval = true;
              }
            }
          }
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
