#pragma once

#include <memory>
#include <glm/glm.hpp>

class Object;

class Ai
{
public:
  Ai();
  ~Ai();
  
  void setObject(Object * p_object);
  Object * getObject();
  
  virtual void updateLogic(glm::i64 p_nanos);
  
private:
  struct Private;
  std::unique_ptr<Private> d;
  
};
