#include "LogicManager.h"

#include "GameState.h"
#include "Object.h"

#include <atomic>

struct LogicManager::Private
{
  std::string name;
  std::vector<Object *> * objectsToUpdate;
  std::atomic<int> currentIndex;
};

LogicManager::LogicManager() :
  d(new Private())
{
  d->name = "LogicManager";
}

void LogicManager::process()
{
  int size = d->objectsToUpdate->size();
  int blocksize = size / 800;
  if (blocksize == 0)
  {
    blocksize = size;
  }
  int currentIndex;
  int lastIndex;
  int limit = 0;
  while (limit != size)
  {
    currentIndex = d->currentIndex.fetch_add(blocksize);
    limit = std::min(size, currentIndex + blocksize);
    for (;
    currentIndex < limit;
      currentIndex++)
    {
      Object * currentObject = d->objectsToUpdate->at(currentIndex);
      currentObject->updateLogic();
    }
  }
}


std::string const & LogicManager::getName() const
{
  return d->name;
}

LogicManager::~LogicManager()
{}

void LogicManager::advance(GameState * p_state)
{
  d->objectsToUpdate = &p_state->objects;
  d->currentIndex.store(0);

  int threadCount = getThreadPool().threadCount();
  for (int thread = 0;
  thread < threadCount;
    thread++)
  {
    getThreadPool().push(std::bind(&LogicManager::process, this));
  }
  process();
  getThreadPool().waitAndDoTasks();
}