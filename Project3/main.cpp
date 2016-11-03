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
#include "Physics/Shapes/PolygonShape.h"
#include "SharedPtr.h"

//FILE _iob[] = { *stdin, *stdout, *stderr };
#include <SDL.h>

#include <fstream>

double fRand(double fMin, double fMax)
{
  double f = (double)rand() / RAND_MAX;
  return fMin + f * (fMax - fMin);
}

void addObject(std::vector<Object*> & p_objects, std::shared_ptr<Asset> p_asset, SpatialTree * tree)
{
  Object * object = new Object();
  
  double power = 4;

  double rand = fRand(0, 2 * 3.1415926);

  float cosCounter = cosf(rand);
  float sinCounter = sinf(rand);
  float rotspeed = 0;
/*if (p_objects.size() == 0)
  {
    rotspeed = 31.415926;
  }*/

  object->setRot(0);
  object->setXPos(cosCounter * OBJTOWORLD * p_objects.size() + OBJTOWORLD * 2);
  object->setYPos(sinCounter * OBJTOWORLD * p_objects.size() + OBJTOWORLD * 2);
  object->updateTransform();
  object->setHalfSize(glm::u64vec2(OBJTOWORLD, OBJTOWORLD));
  object->setAsset(p_asset);
  object->setRotSpeed(rotspeed);
  object->setRot(p_objects.size());
  object->setSpeed(glm::i64vec2(-cosCounter * OBJTOWORLD * std::sqrt(p_objects.size()*2), -sinCounter * OBJTOWORLD * std::sqrt(p_objects.size()*2)));
  /**
  CircleShape * shape = new CircleShape;
  shape->setPos(glm::i64vec2(0, 0));
  shape->setRadius(OBJTOWORLD*2);
  /*/
  PolygonShape * shape = new PolygonShape;
  std::vector<glm::f64vec2> vertices;
  vertices.resize(4);
  vertices[0] = glm::f64vec2(OBJTOWORLD, OBJTOWORLD);
  vertices[1] = glm::f64vec2(-OBJTOWORLD/2, OBJTOWORLD/2);
  vertices[2] = glm::f64vec2(OBJTOWORLD/2, -OBJTOWORLD/2);
  vertices[3] = glm::f64vec2(-OBJTOWORLD, -OBJTOWORLD);

  shape->setVertices(vertices);
  //*/
  Fixture * fixture = new Fixture;
  fixture->density = 0.0000000000000000000001;
  fixture->friction = 0;
  fixture->restitution = 1;
  fixture->object = object;
  fixture->shape = shape;
  
  object->addFixture(fixture);
  object->updateMass();

  tree->addObject(object);
  p_objects.push_back(object);
}

void addObject1(std::vector<Object*> & p_objects, std::shared_ptr<Asset> p_asset, SpatialTree * tree)
{
  Object * object = new Object();

  double power = 4;

  double rand = fRand(0, 2 * 3.1415926);

  float cosCounter = cosf(rand);
  float sinCounter = sinf(rand);
  float rotspeed = 0;
  /*if (p_objects.size() == 0)
  {
  rotspeed = 31.415926;
  }*/

  object->setRot(0);
  object->setXPos(OBJTOWORLD * 2);
  object->setYPos(OBJTOWORLD);
  object->updateTransform();
  object->setHalfSize(glm::u64vec2(OBJTOWORLD, OBJTOWORLD));
  object->setAsset(p_asset);
  object->setRotSpeed(rotspeed);
  object->setRot(p_objects.size());
  object->setSpeed(glm::i64vec2(0, 0));
  /**
  CircleShape * shape = new CircleShape;
  shape->setPos(glm::i64vec2(0, 0));
  shape->setRadius(OBJTOWORLD);
  /*/
  PolygonShape * shape = new PolygonShape;
  std::vector<glm::f64vec2> vertices;
  vertices.resize(4);
  vertices[0] = glm::f64vec2(OBJTOWORLD, OBJTOWORLD);
  vertices[1] = glm::f64vec2(-OBJTOWORLD / 2, OBJTOWORLD / 2);
  vertices[2] = glm::f64vec2(OBJTOWORLD / 2, -OBJTOWORLD / 2);
  vertices[3] = glm::f64vec2(-OBJTOWORLD, -OBJTOWORLD);

  shape->setVertices(vertices);
  //*/
  Fixture * fixture = new Fixture;
  fixture->density = 0.0000000001;
  fixture->friction = 0.5;
  fixture->restitution = 1;
  fixture->object = object;
  fixture->shape = shape;

  object->addFixture(fixture);
  object->updateMass();

  tree->addObject(object);
  p_objects.push_back(object);
}

void addObject2(std::vector<Object*> & p_objects, std::shared_ptr<Asset> p_asset, SpatialTree * tree)
{
  Object * object = new Object();

  double power = 4;

  double rand = fRand(0, 2 * 3.1415926);

  float cosCounter = cosf(rand);
  float sinCounter = sinf(rand);
  float rotspeed = 0;
  /*if (p_objects.size() == 0)
  {
  rotspeed = 31.415926;
  }*/

  object->setRot(0);
  object->setXPos(-OBJTOWORLD * 2);
  object->setYPos(0);
  object->updateTransform();
  object->setHalfSize(glm::u64vec2(OBJTOWORLD, OBJTOWORLD));
  object->setAsset(p_asset);
  object->setRotSpeed(rotspeed);
  object->setRot(p_objects.size());
  object->setSpeed(glm::i64vec2(OBJTOWORLD*2, 0));
  /**
  CircleShape * shape = new CircleShape;
  shape->setPos(glm::i64vec2(0, 0));
  shape->setRadius(OBJTOWORLD);
  /*/
  PolygonShape * shape = new PolygonShape;
  std::vector<glm::f64vec2> vertices;
  vertices.resize(4);
  vertices[0] = glm::f64vec2(OBJTOWORLD, OBJTOWORLD);
  vertices[1] = glm::f64vec2(-OBJTOWORLD / 2, OBJTOWORLD / 2);
  vertices[2] = glm::f64vec2(OBJTOWORLD / 2, -OBJTOWORLD / 2);
  vertices[3] = glm::f64vec2(-OBJTOWORLD, -OBJTOWORLD);

  shape->setVertices(vertices);
  //*/
  Fixture * fixture = new Fixture;
  fixture->density = 0.0000000001;
  fixture->friction = 0.5;
  fixture->restitution = 1;
  fixture->object = object;
  fixture->shape = shape;

  object->addFixture(fixture);
  object->updateMass();

  tree->addObject(object);
  p_objects.push_back(object);
}

void removeObject(std::vector<Object*> & p_objects)
{
  delete p_objects.back();
  p_objects.pop_back();
}

int main(int argc, char ** argv)
{
  SDL_Init(SDL_INIT_EVERYTHING);

  std::vector<Vertex> vertices;

  vertices.emplace_back(glm::vec3(-0.5, 0.5, 0), glm::vec2(0, 0), glm::vec3(0, 0, -1));
  vertices.emplace_back(glm::vec3(-1, -1, 0), glm::vec2(0, 1), glm::vec3(0, 0, -1));
  vertices.emplace_back(glm::vec3(0.5, -0.5, 0), glm::vec2(1, 1), glm::vec3(0, 0, -1));
  vertices.emplace_back(glm::vec3(1, 1, 0), glm::vec2(1, 0), glm::vec3(0, 0, -1));

  int ai = 1;
  int bi = 2;

  SharedPtr<int> a(&ai);
  SharedPtr<int> b(a);
  a = &bi;



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

  Display display(1600, 800, "Screen 1");
  CameraWorldBased camera(glm::i64vec2(0, 0), glm::i32vec2(1600, 800), OBJTOWORLD / 16, 1);
  camera.updateTransform();
  display.setCamera(&camera);

  std::shared_ptr<Asset> fighterAsset(new Asset());
  fighterAsset->setTexture(std::shared_ptr<Texture>(new Texture("./res/texture.png")));
  fighterAsset->setMesh(std::shared_ptr<Mesh>(new Mesh(vertices, indices)));

  std::vector<Object*> objects;

  AABB aabb;
  aabb.setCenter(glm::i64vec2(0, 0));
  aabb.setSize(glm::u64vec2(std::numeric_limits<glm::u64>::max() / 16, std::numeric_limits<glm::u64>::max() / 16));

  GameState state;
  state.focusedObject = nullptr;
  state.displays.push_back(&display);
  state.spatialTree = new QuadTree(aabb);
//*
  for(int index = 0;
  index < 10000;
    index++)
  {
    addObject(objects, fighterAsset, state.spatialTree);
    state.spatialTree = state.spatialTree->top();
  }/*/
  addObject1(objects, fighterAsset, state.spatialTree);
  state.spatialTree = state.spatialTree->top();
  addObject2(objects, fighterAsset, state.spatialTree);
  state.spatialTree = state.spatialTree->top();
//*/
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
