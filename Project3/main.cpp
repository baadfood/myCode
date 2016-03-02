#include <iostream>
#include <GL/glew.h>
#include <GL/gl.h>
#include "Display.h"
#include "Shader.h"
#include "Mesh.h"
#include "Vertex.h"
#include <vector>
#include "Texture.h"
#include "Transform.h"
#include "time.h"
#include "Camera.h"
#include "LightSource.h"
#include "ObjectShader.h"
#include "GenericRenderer.h"
#include "CameraWorldBased.h"
#include "GameState.h"
#include "GameManager.h"
#include "QuadTree.h"
#include "MtQuadTree.h"
#include "InputHandlers/CameraRightClickMoveHandler.inl"
#include "Physics/Collision/Collision.h"
#include "Physics/Shapes/CircleShape.h"

FILE _iob[] = { *stdin, *stdout, *stderr };
#include <SDL.h>

#include <fstream>

double fRand(double fMin, double fMax)
{
  double f = (double)rand() / RAND_MAX;
  return fMin + f * (fMax - fMin);
}
/*

std::atomic<int> s_currentIndex;
std::vector<Object*> s_objectsToCollide;

void moveObjects()
{
  int size = s_objectsToCollide.size();
  int blocksize = size / 800;
  int currentIndex;
  int lastIndex;
  int limit = 0;
  std::vector<Object *> collisions;
  collisions.reserve(200);
  while (limit != size)
  {
    unsigned int ticks = SDL_GetTicks();
    currentIndex = s_currentIndex.fetch_add(blocksize);
    limit = std::min(size, currentIndex + blocksize);
    for (;
    currentIndex < limit;
      currentIndex++)
    {
      s_objectsToCollide[currentIndex]->moveBy(glm::ivec2(rand() % 21 - 10, rand() % 21 - 10));
    }
  }
}

std::vector<Collision *> collisionChecks()
{
  int size = s_objectsToCollide.size();
  int blocksize = size / 800;
  int currentIndex;
  int lastIndex;
  int limit = 0;
  std::vector<Collision *> collisions;
  std::vector<Object *> objectCollisions;
  while(limit != size)
  {
    unsigned int ticks = SDL_GetTicks();
    currentIndex = s_currentIndex.fetch_add(blocksize);
    limit = std::min(size, currentIndex + blocksize);
    for (;
    currentIndex < limit;
    currentIndex++)
    {
      s_objectsToCollide[currentIndex]->getTreeNode()->getObjectsIntersected(s_objectsToCollide[currentIndex]->getAABB(), objectCollisions);
      for (Object * obj : objectCollisions)
      {
        if (obj < s_objectsToCollide[currentIndex])
        {
          collisions.emplace_back(s_objectsToCollide[currentIndex], obj);
        }
      }
    }
  }
  return collisions;
}
*/
void addObject(std::vector<Object*> & p_objects, std::shared_ptr<Asset> p_asset, SpatialTree * tree)
{
  Object * object = new Object();
  
  double power = 4;

  double rand = fRand(0, 2 * 3.1415926);

  float cosCounter = cosf(rand);
  float sinCounter = sinf(rand);

  object->setRot(-rand);
  object->setXPos(cosCounter * OBJTOWORLD * p_objects.size());
  object->setYPos(sinCounter * OBJTOWORLD * p_objects.size());
  object->updateTransform();
  object->setHalfSize(glm::u64vec2(OBJTOWORLD, OBJTOWORLD));
  object->setAsset(p_asset);
  object->setRotSpeed(0);
  object->setSpeed(glm::i64vec2(-cosCounter * OBJTOWORLD * std::sqrt(p_objects.size()), -sinCounter * OBJTOWORLD * std::sqrt(p_objects.size())));
  
  CircleShape * circleShape = new CircleShape;
  circleShape->pos = glm::i64vec2(0, 0);
  circleShape->radius = OBJTOWORLD;
 
  Fixture * fixture = new Fixture;
  fixture->density = 0.000001;
  fixture->friction = 1;
  fixture->restitution = 1;
  fixture->object = object;
  fixture->shape = circleShape;
  
  object->addFixture(fixture);
  
  tree->addObject(object);
  p_objects.push_back(object);
}

void removeObject(std::vector<Object*> & p_objects)
{
  delete p_objects.back();
  p_objects.pop_back();
}
/*
int main(int argc, char **argv)
{
  //Quadtree test;
  QuadTree * tree = new QuadTree(AABB(glm::i64vec2(0, 0), glm::u64vec2(128, 128)));

  const int count = 100000;

  std::vector<Object *> objects;
  objects.reserve(count);

  int rows = sqrt(count);

  unsigned int ticks = SDL_GetTicks();

  for(int index = 0;
  index < count;
    index++)
  {
  	Object * obj = new Object();
    objects.push_back(obj);
    obj->setName(std::string("Obj ").append(std::to_string(index)));
    obj->moveTo(glm::i64vec2((index%rows) * 10, (index/rows) * 10));
    obj->setHalfSize(glm::u64vec2(1, 1));
  }

  std::cout << "Adding to tree\n";

  for(int index = 0;
  index < count;
    index++)
  {
    tree = tree->addObject(objects[index]);
  }

//  tree->top()->updateTree();
  getThreadPool().waitAndDoTasks();

  if (tree->sanityCheck() == false)
  {
    std::cout << "Tree error\n";
  }

  AABB query;
  query.m_pos = glm::i64vec2(0, 0);
  query.m_halfSize = glm::u64vec2(10, 10);
  std::vector<Object*> result;

  tree->getObjectsIn(query, result);
  std::cout << "Query1 result\n";
  for(auto iter = result.begin();
  iter != result.end();
    iter++)
  {
    std::cout << (*iter)->getName() << " ";
    (*iter)->getAABB().print();
  }

  tree->getObjectsAt(glm::i64vec2(unsigned int(1), 1), result);
  std::cout << "Query2 result\n";
  for(auto iter = result.begin();
  iter != result.end();
    iter++)
  {
    std::cout << (*iter)->getName() << " ";
    (*iter)->getAABB().print();
  }

  Object * closest = tree->getObjectClosestHitObject(glm::i64vec2(1, 1));
  std::cout << "Query3 result\n";
  if (closest)
  {
	  std::cout << closest->getName() << " ";
	  closest->getAABB().print();
  }

  tree->getObjectsHitInPriority(glm::i64vec2(1, 1), result);
  std::cout << "Query4 result\n";
  for(auto iter = result.begin();
  iter != result.end();
    iter++)
  {
    std::cout << (*iter)->getName() << " ";
    (*iter)->getAABB().print();
  }

  s_objectsToCollide = objects;

  unsigned int collisionTotal = 0;
  unsigned int totalUpdateTimeSpent = SDL_GetTicks();
  for(int updateIndex = 0;
  updateIndex < 100;
    updateIndex++)
  {
    std::cout << "Updating positions\n";
    getThreadPool().push(std::bind(moveObjects));
    getThreadPool().push(std::bind(moveObjects));
    getThreadPool().push(std::bind(moveObjects));
    getThreadPool().push(std::bind(moveObjects));
    getThreadPool().push(std::bind(moveObjects));
    getThreadPool().push(std::bind(moveObjects));
    getThreadPool().push(std::bind(moveObjects));
    getThreadPool().push(std::bind(moveObjects));
    moveObjects();
    getThreadPool().waitAndDoTasks();
        for(int index = 0;
    index < count;
      index++)
    {
      objects[index]->moveBy(glm::ivec2(rand() % 21 - 10, rand() % 21 - 10));
    }
    for (int index = 0;
    index < count;
      index++)
    {
      objects[index]->updateTree();
    }
    std::cout << "Querying collisions\n";
    unsigned int collisionQuery = SDL_GetTicks();
    s_currentIndex.store(0);

    std::vector<std::future<std::vector<Collision>>> tasks;

    tasks.push_back(getThreadPool().push(std::bind(collisionChecks)));
    tasks.push_back(getThreadPool().push(std::bind(collisionChecks)));
    tasks.push_back(getThreadPool().push(std::bind(collisionChecks)));
    tasks.push_back(getThreadPool().push(std::bind(collisionChecks)));
    tasks.push_back(getThreadPool().push(std::bind(collisionChecks)));
    tasks.push_back(getThreadPool().push(std::bind(collisionChecks)));
    tasks.push_back(getThreadPool().push(std::bind(collisionChecks)));
    tasks.push_back(getThreadPool().push(std::bind(collisionChecks)));

    std::vector<Collision> collisions(collisionChecks());

/*    for (int index = 0;
    index < count;
      index++)
    {
      std::vector<Object*> collisions;
      objects[index]->getTreeNode()->getObjectsIntersected(objects[index]->getAABB(), collisions);
    }
    //    tree->top()->updateTree();

    getThreadPool().waitAndDoTasks();
    unsigned int ticksNow = SDL_GetTicks();
    collisionTotal += ticksNow - collisionQuery;
    std::cout << "Collisions done in " << ticksNow - collisionQuery << std::endl;
  }
  totalUpdateTimeSpent = SDL_GetTicks() - totalUpdateTimeSpent;

  if (tree->top()->sanityCheck() == false)
  {
    std::cout << "Tree error\n";
  }
  std::cout << "Done Updating positions\n";

  std::cout << "Ticks taken " << totalUpdateTimeSpent << "Collision checks" << collisionTotal << std::endl;

  if (tree->sanityCheck() == false)
  {
    std::cout << "Tree error\n";
  }

  return 0;
}
*/

int main(int argc, char ** argv)
{
  SDL_Init(SDL_INIT_EVERYTHING);

  std::vector<Vertex> vertices;

  vertices.emplace_back(glm::vec3(-1, 1, 0), glm::vec2(0, 0), glm::vec3(0, 0, -1));
  vertices.emplace_back(glm::vec3(-1, -1, 0), glm::vec2(0, 1), glm::vec3(0, 0, -1));
  vertices.emplace_back(glm::vec3(1, -1, 0), glm::vec2(1, 1), glm::vec3(0, 0, -1));
  vertices.emplace_back(glm::vec3(1, 1, 0), glm::vec2(1, 0), glm::vec3(0, 0, -1));


  unsigned int indics[] = { 0, 1, 2,
    0, 2, 3
  };
  std::vector<unsigned int> indices;
  for(int index = 0;
  index < sizeof(indics) / sizeof(unsigned int);
    index++)
  {
    indices.push_back(indics[index]);
  }

  Display display(800, 600, "Screen 1");
  CameraWorldBased camera(glm::i64vec2(0, 0), glm::i32vec2(800, 600), OBJTOWORLD * 10, 1);
  camera.updateTransform();
  display.setCamera(&camera);

  std::shared_ptr<Asset> fighterAsset(new Asset());
  fighterAsset->setTexture(std::shared_ptr<Texture>(new Texture("./res/fighter.png")));
  fighterAsset->setMesh(std::shared_ptr<Mesh>(new Mesh(vertices, indices)));

  std::vector<Object*> objects;

  AABB aabb;
  aabb.setCenter(glm::i64vec2(0, 0));
  aabb.setSize(glm::u64vec2(67108864, 67108864));

  GameState state;
  state.focusedObject = nullptr;
  state.displays.push_back(&display);
  state.spatialTree = new QuadTree(aabb);
  for(int index = 0;
  index < 30000;
    index++)
  {
    addObject(objects, fighterAsset, state.spatialTree);
    state.spatialTree = state.spatialTree->top();
  }

  state.objects = objects;

  state.spatialTree = state.spatialTree->top();
  state.prevFrameTime = SDL_GetTicks();
  state.currentFrameTime = SDL_GetTicks();

  GameManager manager;
  manager.setGameState(&state);

  unsigned int tickStart = SDL_GetTicks();
  unsigned int frames = 0;
  unsigned int lastTicks = SDL_GetTicks();

  while(display.isClosed() == false)
  {
    unsigned int ticksNow = SDL_GetTicks();
    if(lastTicks + 20 > ticksNow)
    {
      SDL_Delay(lastTicks + 20 - ticksNow);
      continue;
    }

    manager.progressFrame();

  }

  vertices.clear();

  SDL_Quit();

  return 0;
}

/*

int main(int argc, char ** argv)
{
	SDL_Init(SDL_INIT_EVERYTHING);

	Display display(1920, 1080, "Screen 1");

  std::vector<Vertex> vertices;

  vertices.emplace_back(glm::vec3(-1, -1, -1), glm::vec2(1, 0), glm::vec3(0, 0, -1));
  vertices.emplace_back(glm::vec3(-1, 1, -1), glm::vec2(0, 0), glm::vec3(0, 0, -1));
  vertices.emplace_back(glm::vec3(1, 1, -1), glm::vec2(0, 1), glm::vec3(0, 0, -1));
  vertices.emplace_back(glm::vec3(1, -1, -1), glm::vec2(1, 1), glm::vec3(0, 0, -1));

  vertices.emplace_back(glm::vec3(-1, -1, 1), glm::vec2(1, 0), glm::vec3(0, 0, 1));
  vertices.emplace_back(glm::vec3(-1, 1, 1), glm::vec2(0, 0), glm::vec3(0, 0, 1));
  vertices.emplace_back(glm::vec3(1, 1, 1), glm::vec2(0, 1), glm::vec3(0, 0, 1));
  vertices.emplace_back(glm::vec3(1, -1, 1), glm::vec2(1, 1), glm::vec3(0, 0, 1));

  vertices.emplace_back(glm::vec3(-1, -1, -1), glm::vec2(0, 1), glm::vec3(0, -1, 0));
  vertices.emplace_back(glm::vec3(-1, -1, 1), glm::vec2(1, 1), glm::vec3(0, -1, 0));
  vertices.emplace_back(glm::vec3(1, -1, 1), glm::vec2(1, 0), glm::vec3(0, -1, 0));
  vertices.emplace_back(glm::vec3(1, -1, -1), glm::vec2(0, 0), glm::vec3(0, -1, 0));

  vertices.emplace_back(glm::vec3(-1, 1, -1), glm::vec2(0, 1), glm::vec3(0, 1, 0));
  vertices.emplace_back(glm::vec3(-1, 1, 1), glm::vec2(1, 1), glm::vec3(0, 1, 0));
  vertices.emplace_back(glm::vec3(1, 1, 1), glm::vec2(1, 0), glm::vec3(0, 1, 0));
  vertices.emplace_back(glm::vec3(1, 1, -1), glm::vec2(0, 0), glm::vec3(0, 1, 0));

  vertices.emplace_back(glm::vec3(-1, -1, -1), glm::vec2(1, 1), glm::vec3(-1, 0, 0));
  vertices.emplace_back(glm::vec3(-1, -1, 1), glm::vec2(1, 0), glm::vec3(-1, 0, 0));
  vertices.emplace_back(glm::vec3(-1, 1, 1), glm::vec2(0, 0), glm::vec3(-1, 0, 0));
  vertices.emplace_back(glm::vec3(-1, 1, -1), glm::vec2(0, 1), glm::vec3(-1, 0, 0));

  vertices.emplace_back(glm::vec3(1, -1, -1), glm::vec2(1, 1), glm::vec3(1, 0, 0));
  vertices.emplace_back(glm::vec3(1, -1, 1), glm::vec2(1, 0), glm::vec3(1, 0, 0));
  vertices.emplace_back(glm::vec3(1, 1, 1), glm::vec2(0, 0), glm::vec3(1, 0, 0));
  vertices.emplace_back(glm::vec3(1, 1, -1), glm::vec2(0, 1), glm::vec3(1, 0, 0));

  unsigned int indics[] = { 0, 1, 2,
    0, 2, 3,

    6, 5, 4,
    7, 6, 4,

    10, 9, 8,
    11, 10, 8,

    12, 13, 14,
    12, 14, 15,

    16, 17, 18,
    16, 18, 19,

    22, 21, 20,
    23, 22, 20
  };
  std::vector<unsigned int> indices;
  for (int index = 0;
  index < sizeof(indics) / sizeof(unsigned int);
    index++)
  {
    indices.push_back(indics[index]);
  }

  Mesh mesh(vertices, indices);
  Mesh monkey("res\\monkey.obj");

  Texture texture("res\\F5S4.png");

  Shader shader("res\\basicShader");
  Shader objShader("res\\objectShader");
  Shader objShader2("res\\objectShader2");
  GenericRenderer renderer;

  Transform transform;

  std::vector<Object*> objects;

//  Camera camera(glm::vec3(0, 0, -3), 90, float(800.0f / 600.0f), 0.01, 1000.0);
  CameraWorldBased camera(glm::i64vec2(0, 0), glm::i32vec2(1920, 1080), 67108864, 1);
  camera.updateTransform();

  LightSource light;
  light.position = glm::vec3(0, 0, 10);
  light.intensities = glm::vec3(40000000000, 40000000000, 40000000000);
  light.attenuation = 0.00000000000000000000000000000000000000000000001;
  light.ambienCoefficient = 1;

  renderer.setCamera(&camera);
//  renderer.setLightSource(&light);
//  renderer.setMesh(&mesh);
  renderer.setShader(&objShader2);
//  renderer.setTexture(&texture);

  float counter = 0.0f;

  unsigned int tickStart = SDL_GetTicks();
  unsigned int frames = 0;
  unsigned int lastTicks = SDL_GetTicks();

  for (int index = 0;
  index < 20000;
    index++)
  {
    addObject(objects);
  }

  float density = 4;

	while (display.isClosed() == false)
	{
    unsigned int ticksNow = SDL_GetTicks();
    if(lastTicks + 10 > ticksNow)
    {
      SDL_Delay(lastTicks + 10 - ticksNow);
      continue;
    }
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent))
    {
      glm::mat4 transform;
      glm::vec4 pos;
      glm::i64vec2 worldPos;
      switch (sdlEvent.type)
      {
        case SDL_MOUSEBUTTONDOWN:
        worldPos = camera.screenToWorld(glm::i32vec2(sdlEvent.motion.x, sdlEvent.motion.y));
        std::cout << "Mouse X:" << worldPos.x << " Y:" << worldPos.y << std::endl;
        break;
        case SDL_KEYDOWN:
        switch (sdlEvent.key.keysym.sym)
        {
          case SDLK_w:
          camera.moveBy(glm::i64vec2(0, camera.getWorldPerPixel() * 10));
          camera.updateTransform();
          std::cout << "Cam X:" << camera.getPos().x << " Y:" << camera.getPos().y << std::endl;
          break;
          case SDLK_s:
          camera.moveBy(glm::i64vec2(0, -camera.getWorldPerPixel() * 10));
          camera.updateTransform();
          std::cout << "Cam X:" << camera.getPos().x << " Y:" << camera.getPos().y << std::endl;
          break;
          case SDLK_a:
          camera.moveBy(glm::i64vec2(-camera.getWorldPerPixel() * 10, 0));
          camera.updateTransform();
          std::cout << "Cam X:" << camera.getPos().x << " Y:" << camera.getPos().y << std::endl;
          break;
          case SDLK_d:
          camera.moveBy(glm::i64vec2(camera.getWorldPerPixel() * 10, 0));
          camera.updateTransform();
          std::cout << "Cam X:" << camera.getPos().x << " Y:" << camera.getPos().y << std::endl;
          break;
          case SDLK_q:
          camera.setWorldPerPixel(camera.getWorldPerPixel() * 2);
          camera.updateTransform();
          camera.getAABB().print();
          break;
          case SDLK_e:
          camera.setWorldPerPixel(camera.getWorldPerPixel() / 2);
          camera.updateTransform();
          camera.getAABB().print();
          break;
          case SDLK_r:
          camera.setExposure(camera.getExposure() * 2);
          std::cout << "Cam Exp:" << camera.getExposure() << std::endl;
          break;
          case SDLK_f:
          camera.setExposure(camera.getExposure() / 2);
          std::cout << "Cam Exp:" << camera.getExposure() << std::endl;
          break;
          case SDLK_PLUS:
          density++;
          break;
          case SDLK_MINUS:
          density--;
          case SDLK_UP:
          renderer.setShader(&objShader2);
          break;
          case SDLK_DOWN:
          renderer.setShader(&objShader);
          break;
        }
        break;
      }
    }

    display.clear(0.0, 0.0, 0.0, 1.0);

    frames++;



    for (int index = 0;
    index < objects.size();
      index++)
    {
      Object * object = objects[index];
      float rot = object->getRot();
      rot += float(ticksNow - lastTicks) / 100000;
      object->setRot(rot);
      double srtqIndex = sqrt(index);

      object->moveTo(glm::i64vec2(cosf(rot + 3.1415926/2) * OBJTOWORLD * srtqIndex * density, sinf(rot + 3.1415926/2)* OBJTOWORLD * srtqIndex * density));
    }

//    renderer.renderObjects(objects);
    display.update();


    counter += 0.003f * (ticksNow - lastTicks);

    if (ticksNow - tickStart > 1000)
    {
      std::cout << "FPS " << static_cast<float>(frames * 1000) / static_cast<float>(ticksNow - tickStart) << std::endl;
      tickStart = ticksNow;
      frames = 0;
    }

    lastTicks = ticksNow;

	}

  vertices.clear();

	SDL_Quit();

	return 0;
}

*/