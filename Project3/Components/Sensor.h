#pragma once

#include "Components/Component.h"

class Sensor: public Component
{
public:
  Sensor();
  ~Sensor();
  
  virtual void updateLogic(glm::u64 p_nanos);
  void addSensorFixture(Fixture* p_sensorFixture);
  
  std::vector<Contact *> const & getSensorContacts();
  
private:
  struct Private;
  std::unique_ptr<Private> d;
};