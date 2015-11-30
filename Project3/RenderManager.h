#pragma once

#include "Manager.h"
#include <memory>
struct GameState;

class RenderManager : public Manager
{
public:
  RenderManager();
  virtual ~RenderManager();
  virtual std::string const & getName() const;

  virtual void advance(GameState * p_state);

protected:
private:
  struct Private;
  std::unique_ptr<Private> d;
};

