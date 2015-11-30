#include "LogicManager.h"

#include "GameState.h"

struct LogicManager::Private
{
  std::string name;
};

LogicManager::LogicManager() :
  d(new Private())
{
  d->name = "LogicManager";
}

std::string const & LogicManager::getName() const
{
  return d->name;
}

LogicManager::~LogicManager()
{}

void LogicManager::advance(GameState * p_state)
{
  // TODO Implement logic
}