#include "utils.h"

SpinlockThreadpool & getThreadPool()
{
  static SpinlockThreadpool s_threadpool;
  return s_threadpool;
}
