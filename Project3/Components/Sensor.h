#pragma once

#include "Components/Component.h"

class Sensor: public Component
{
public:
  Sensor();
  ~Sensor();
  
  void setEnabled(bool p_value);
  
  virtual void updateLogic(glm::u64 p_nanos);
  virtual void addFixture(Fixture * p_fix);
  void addSensorFixture(Fixture* p_sensorFixture);
  virtual void computeAabb();

  std::vector<Contact *> const & getSensorContacts();
  
private:
  struct Private;
  std::unique_ptr<Private> d;
};