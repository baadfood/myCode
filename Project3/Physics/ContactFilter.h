#ifndef ContactFilter_h_
#define ContactFilter_h_

class Fixture;
class ContactFilter
{
public:
  virtual ~ContactFilter();

  virtual bool shouldContact(Fixture * p_fix1, Fixture * p_fix2) = 0;
};

#endif