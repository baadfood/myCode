#include "PhysicsManager.h"

#include "GameState.h"

#include "SpinlockThreadpool.h"
#include "SpatialTree.h"

#include "ShapeCollisionDetector.h"
#include "Object.h"

#include <atomic>


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
  int currentIndex;
  int lastIndex;
  int limit = 0;
  std::vector<Object *> collisions;
  collisions.reserve(200);
  while (limit != size)
  {
    unsigned int ticks = SDL_GetTicks();
    currentIndex =  d->currentIndex.fetch_add(blocksize);
    limit = std::min(size, currentIndex + blocksize);
    for (;
    currentIndex < limit;
      currentIndex++)
    {
      d->objectsToUpdate->at(currentIndex)->advance(d->nanosToAdvance);
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
      for (Object * obj : objectCollisions)
      {
        if (obj < currentObject)
        {
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
  bool sortPrimaries(std::vector<Object*> const & p_first, std::vector<Object*> const & p_second)
  {
    return p_first.front() < p_second.front();
  }
}

void PhysicsManager::advance(GameState * p_state)
{
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

  std::vector<std::future<ContactsData *>> tasks;

  int workerCount = getThreadPool().threadCount();
  for (int thread = 0;
  thread < workerCount;
    thread++)
  {
    tasks.emplace_back(getThreadPool().push(std::bind(&PhysicsManager::getContacts, this)));
  }
  std::vector<std::vector<Object *>> primaries;
  std::vector<Contact *> contacts;
  ContactsData * contactData = nullptr;
  while (tasks.empty() == false)
  {
    for (auto iter = tasks.begin();
    iter != tasks.end();
      iter++)
    {
      if(iter->_Is_ready()
      && iter->valid())
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
      }
    }
  }
  std::sort(primaries.begin(), primaries.end(), sortPrimaries);

  std::vector<Object *> primariesJoined;

  size_t size = 0;
  for (std::vector<Object*> group : primaries)
  {
    size += group.size();
  }

  primariesJoined.resize(size);

  for (std::vector<Object*> group : primaries)
  {
    primariesJoined.insert(primariesJoined.end(), group.begin(), group.end());
  }

  std::vector<std::vector<Object*>> islands;

  while(primariesJoined.empty() == false)
  {
    std::vector<Object*> island;
    island.push_back(primaries.front().front());
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

  std::cout << "Such islands: " << islands.size() << std::endl;

  // is it possible to do multithreaded collision responses?
}