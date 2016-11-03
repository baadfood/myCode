#include "PhysicsManager.h"

#include "GameState.h"

#include "SpinlockThreadpool.h"
#include "SpatialTree.h"

#include "ShapeCollisionDetector.h"
#include "Object.h"
#include "Physics/Shape.h"
#include "Physics/Collision/CircleToCircleCollision.h"
#include "Physics/Collision/PolygonToPolygonCollision.h"
#include "Physics/CollisionIsland.h"
#include "QuadTree.h"

#include <atomic>
#include <map>


struct PhysicsManager::Private
{
  glm::u64 nanosToAdvance;
  std::string name;
  std::atomic<int> currentIndex;
  std::vector<Object*> * objectsToUpdate;
  ShapeCollisionDetector collisionDetector;
};

struct PhysicsManager::ContactsData
{
  std::vector<Contact *> contacts;
  std::vector<Object *> primaryObjects;
};

PhysicsManager::PhysicsManager():
d(new Private())
{
  d->name = "PhysicsManager";
  d->collisionDetector.registerCollisionHandler(Shape::eCircle, Shape::eCircle, new CircleToCircleCollision);
  d->collisionDetector.registerCollisionHandler(Shape::ePolygon, Shape::ePolygon, new PolygonToPolygonCollision);
}

PhysicsManager::~PhysicsManager()
{
}

std::string const & PhysicsManager::getName() const
{
  return d->name;
}

void PhysicsManager::moveObjects()
{
  int size = d->objectsToUpdate->size();
  int blocksize = size / 800;
  if(blocksize == 0)
  {
    blocksize = size;
  }
  int currentIndex;
  int lastIndex;
  int limit = 0;
  while (limit != size)
  {
    currentIndex =  d->currentIndex.fetch_add(blocksize);
    limit = std::min(size, currentIndex + blocksize);
    for (;
    currentIndex < limit;
      currentIndex++)
    {
      d->objectsToUpdate->at(currentIndex)->advance(d->nanosToAdvance);
      d->objectsToUpdate->at(currentIndex)->updateAabb();
      d->objectsToUpdate->at(currentIndex)->clearContacts();
    }
  }
}

bool PhysicsManager::checkCollision(Object * p_object1, Object * p_object2, std::vector<Contact *> & p_results)
{
  return d->collisionDetector.getCollisions(p_object1, p_object2, p_results);
}

PhysicsManager::ContactsData * PhysicsManager::getContacts()
{
  int size = d->objectsToUpdate->size();
  int blocksize = size / 800;
  if(blocksize == 0)
  {
    blocksize = size;
  }
  int currentIndex;
  int lastIndex;
  int limit = 0;

  ContactsData * retval = new ContactsData();

  std::vector<Object *> objectCollisions;
  while (limit != size)
  {
    currentIndex = d->currentIndex.fetch_add(blocksize);
    limit = std::min(size, currentIndex + blocksize);
    for (;
    currentIndex < limit;
      currentIndex++)
    {
      Object * currentObject = d->objectsToUpdate->at(currentIndex);
      currentObject->getTreeNode()->getObjectsIntersected(currentObject->getAABB(), objectCollisions);
      bool firstCollision = true;
      std::vector<Contact*> const & contacts = currentObject->getContacts();
      for (Object * obj : objectCollisions)
      {
        if (currentObject < obj)
        {
/*          for (Contact const * contact : contacts)
          {
            if (contact->fixtures[1].object == obj
              || contact->fixtures[0].object == obj)
            {
              // already colliding
              continue;
            }
          }*/
          if (checkCollision(currentObject, obj, retval->contacts))
          {
            if (firstCollision)
            {
              firstCollision = false;
              retval->primaryObjects.push_back(currentObject);
            }
          }
        }
      }
    }
  }
  return retval;
}

void PhysicsManager::correctPositions()
{
  int size = d->objectsToUpdate->size();
  int blocksize = size / 800;
  if(blocksize == 0)
  {
    blocksize = size;
  }
  int currentIndex;
  int lastIndex;
  int limit = 0;
  std::vector<Contact *> * contacts = new std::vector<Contact *>();
  std::vector<Object *> objectCollisions;
  while (limit != size)
  {
    unsigned int ticks = SDL_GetTicks();
    currentIndex = d->currentIndex.fetch_add(blocksize);
    limit = std::min(size, currentIndex + blocksize);
    for (;
    currentIndex < limit;
      currentIndex++)
    {
      Object * currentObject = d->objectsToUpdate->at(currentIndex);
      currentObject->getTreeNode()->getObjectsIntersected(currentObject->getAABB(), objectCollisions);
      for (Object * obj : objectCollisions)
      {
        if (obj < currentObject)
        {
          checkCollision(currentObject, obj, *contacts);
        }
      }
    }
  }
}

namespace
{
  bool toiSort(Contact const * p_contact1, Contact const * p_contact2)
  {
    if(p_contact1->timeOfImpact < p_contact2->timeOfImpact)
    {
      return true;
    }
    return false;
  }
}

void PhysicsManager::processIsland(std::vector< Object* > const & p_island)
{
  unsigned int startTicks = SDL_GetTicks();
  std::vector<Contact *> contacts;
  contacts.reserve(p_island.size() * 2);

  for(Object * obj : p_island)
  {
    std::vector<Contact*> const & objContacts = obj->getContacts();
    contacts.reserve(objContacts.size() + contacts.capacity());
    for(Contact * contact : objContacts)
    {
      contacts.push_back(contact);
    }
  }
  unsigned int ticksNow = SDL_GetTicks();
  std::cout << "Physicsmanager Got island objects " << ticksNow - startTicks << std::endl;
  startTicks = ticksNow;

  std::sort(contacts.begin(), contacts.end(), toiSort);
  ticksNow = SDL_GetTicks();
  std::cout << "Physicsmanager sorted island objects " << ticksNow - startTicks << std::endl;
  startTicks = ticksNow;
  for(Contact * contact : contacts)
  {
    contact->applyImpulse();
  }
  ticksNow = SDL_GetTicks();
  std::cout << "Physicsmanager applied impulses to objects" << ticksNow - startTicks << std::endl;
  startTicks = ticksNow;
  for (Contact * contact : contacts)
  {
//    contact->positionCorrectionPre();
  }
  for (Contact * contact : contacts)
  {
//     contact->positionCorrection();
  }
}

namespace
{
  bool sortPrimaries(std::vector<Object*> const & p_first, std::vector<Object*> const & p_second)
  {
    if(p_second.empty())
    {
      if (p_first.empty())
      {
        return &p_first < &p_second;
      }
      return true;
    }
    if (p_first.empty())
    {
      if (p_second.empty())
      {
        return &p_first < &p_second;
      }
      return false;
    }
    return p_first.front() < p_second.front();
  }
}

void PhysicsManager::advance(GameState * p_state)
{
  unsigned int startTicks = SDL_GetTicks();
  d->currentIndex.store(0);
  d->objectsToUpdate = &p_state->objects;
  d->nanosToAdvance = p_state->ticksAdvanced * 1e6;


  int threadCount = getThreadPool().threadCount();
  for (int thread = 0;
  thread < threadCount;
    thread++)
  {
    getThreadPool().push(std::bind(&PhysicsManager::moveObjects, this));
  }
  moveObjects();
  getThreadPool().waitAndDoTasks();
  d->currentIndex.store(0);

  unsigned int ticksNow = SDL_GetTicks();
  std::cout << "Physicsmanager moved objects in " << ticksNow - startTicks << std::endl;
  startTicks = ticksNow;


  for(auto iter = d->objectsToUpdate->begin();
      iter != d->objectsToUpdate->end();
      iter++)
  {
    (*iter)->updateTree();
  }

  ticksNow = SDL_GetTicks();
  std::cout << "Physicsmanager updated tree in " << ticksNow - startTicks << std::endl;
  startTicks = ticksNow;


  std::vector<std::future<ContactsData *>> tasks;

  int workerCount = getThreadPool().threadCount();
  for (int thread = 0;
  thread < workerCount;
    thread++)
  {
    tasks.emplace_back(getThreadPool().push(std::bind(&PhysicsManager::getContacts, this)));
  }
  std::vector<std::vector<Object *> > primaries;
  std::vector<Contact *> contacts;
  ContactsData * contactData = nullptr;
  getThreadPool().waitAndDoTasks();
  while (tasks.empty() == false)
  {
    for (auto iter = tasks.begin();
         iter != tasks.end();
        )
    {
      if(iter->valid()
      && iter->wait_for(std::chrono::seconds(1)) == std::future_status::ready
      )
      {
        contactData = iter->get();
        contacts.resize(contacts.size() + contactData->contacts.size());
        for (Contact * contact : contactData->contacts)
        {
          contacts.push_back(std::move(contact));
        }
        primaries.push_back(contactData->primaryObjects);
        delete contactData;
        std::swap(tasks.back(), *iter);
        tasks.pop_back();
        if (tasks.empty())
        {
          break;
        }
      }
    }
  }

  ticksNow = SDL_GetTicks();
  std::cout << "Physicsmanager got contacts2 in " << ticksNow - startTicks << std::endl;
  startTicks = ticksNow;
  for (Object* object : *d->objectsToUpdate)
  {
    object->updateIsland();
  }

  std::set<CollisionIsland *> islands;
  for (Object* object : *d->objectsToUpdate)
  {
    CollisionIsland * island = object->getCollisionIsland();
    if (island)
    {
      islands.insert(island);
    }
  }
  /*
  for (std::vector<Object*> group : primaries)
  {
    for (Object* object : group)
    {
      object->updateIsland();
    }
  }

  std::set<CollisionIsland *> islands;
  for (std::vector<Object*> group : primaries)
  {
    for (Object* object : group)
    {
      islands.insert(object->getCollisionIsland());
    }
  }
  */
  /*
  std::sort(primaries.begin(), primaries.end(), sortPrimaries);

  std::vector<Object *> primariesJoined;

  size_t size = 0;
  for (std::vector<Object*> group : primaries)
  {
    size += group.size();
  }

  primariesJoined.reserve(size);

  for (std::vector<Object*> group : primaries)
  {
    primariesJoined.insert(primariesJoined.end(), group.begin(), group.end());
  }

  std::vector<std::vector<Object*>> islands;

  for(Object * currentObj : primariesJoined)
  {
    if (currentObj->getCollisionIsland() == nullptr)
    {
      std::vector<Object*> island;
      island.push_back(currentObj);
      island.front()->getConnectedObjects(island);

      islands.push_back(island);
    }
  }


  */
  /*
  std::vector<std::vector<Object*>> islands2;

  if (islands.size() > 1)
  {
    auto prevIter = islands.begin();
    for (auto iter = islands.begin();
         iter != islands.end();
      )
    {
      CollisionIsland const * mainIsland = iter->at(0)->getCollisionIsland();
      for (auto iter2 = mainIsland->connectedIslands.begin();
      iter2 != mainIsland->connectedIslands.end();
        iter2++)
      {
        auto iter3 = iter;
        ++iter3;
        for (;
        iter3 != islands.end();
          iter3++)
        {
          if (iter3->at(0)->getCollisionIsland() == (*iter2))
          {
            iter->insert(iter->end(), iter3->begin(), iter3->end());
          }
        }
      }
    }
  }
  */
  /*
  while(primariesJoined.empty() == false)
  {
    std::vector<Object*> island;
    island.push_back(primariesJoined.front());
    island.front()->getConnectedObjects(island);

    std::swap(primariesJoined.front(), primariesJoined.back());
    primariesJoined.pop_back();

    for (Object * islandObject : island)
    {
      // Maybe i could know the index, somehow?
      mika::removeOne(primariesJoined, islandObject);
    }
    islands.push_back(island);
  }
/*
  std::vector<std::vector<Object*>> islands;

  CollisionIsland::sortObjects(islands, *(d->objectsToUpdate));
  */

  ticksNow = SDL_GetTicks();
   std::cout << "Physicsmanager got islands in " << ticksNow - startTicks << std::endl;
  startTicks = ticksNow;

//   std::cout << "Such islands: " << islands.size() << std::endl;
  
  for(auto island = islands.begin();
      island != islands.end();
      island++)
  {
/*    if ((*island)->objects.size() > 200)
    {
      std::cout << "OMG TOO MANY OBJECTS\n";
    }
    std::cout << "Such island: " << (*island)->objects.size() << std::endl;*/
    getThreadPool().push(std::bind(&PhysicsManager::processIsland, this, (*island)->objects));
  }
  getThreadPool().waitAndDoTasks();

  ticksNow = SDL_GetTicks();
   std::cout << "Physicsmanager processed islands in " << ticksNow - startTicks << std::endl;
  startTicks = ticksNow;
}