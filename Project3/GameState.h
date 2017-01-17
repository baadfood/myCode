#pragma once

#include <SDL.h>
#include <memory>
#include <vector>
#include <unordered_set>
#include "utils.h"

class SpatialTree;
class UserInputHandler;
class Object;
class Display;
class QuadTree;

struct GameState
{
  // Display Stuff
  std::vector<Display *> displays;

  // Time stuff
  Uint32 prevFrameTime;
  Uint32 currentFrameTime;
  Uint32 ticksAdvanced;

  // Object stuff
  QuadTree * spatialTree;
  std::vector<Object*> objects;

  // Input handling stuff
  std::vector<UserInputHandler *> globalHandlers;
  Object * focusedObject;
  unsigned int focusedObjectIndex;
  std::unordered_set<SDL_Keysym> keysHeldDown;
  //std::shared_ptr<Object> focusedObject;

};