#pragma once

#include "Manager.h"
#include "Physics/Collision/Collision.h"

#include <memory>
#include <vector>

struct GameState;
class Contact;

class PhysicsManager: public Manager
{
public:
  struct ContactsData;

  PhysicsManager();
  virtual ~PhysicsManager();

  virtual std::string const & getName() const;

  virtual void advance(GameState * p_state);
  virtual void moveObjects();
  virtual ContactsData * getContacts();
  virtual bool checkCollision(Object * p_object1, Object * p_object2, std::vector<Contact *> & p_results);
  virtual void correctPositions();
  virtual void processIsland(std::vector< Object* > const & p_island);

protected:
private:
  struct Private;
  std::unique_ptr<Private> d;
};

