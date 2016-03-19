#ifndef CollisionIsland_h_
#define CollisionIsland_h_

#include <boost/lockfree/queue.hpp>
#include <set>

class CollisionIsland
{
//  static boost::lockfree::queue<CollisionIsland*> s_buffer;
public:
  static CollisionIsland * newCollisionIsland()
  {
    CollisionIsland * island;
//    if (s_buffer.pop(island) == false)
    {
      island = new CollisionIsland();
    }
    return island;
  }

  static void freeIsland(CollisionIsland * p_island)
  {
    delete p_island;
//    s_buffer.push(p_island);
  }

  std::set<CollisionIsland*> connectedIslands;
};

#endif