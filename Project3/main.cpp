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
#include "Components/Component.h"
#include "AI/BasicAi.h"

//FILE _iob[] = { *stdin, *stdout, *stderr };
#include <SDL.h>

#include <fstream>

double fRand(double fMin, double fMax)
{
  double f = (double)rand() / RAND_MAX;
  return fMin + f * (fMax - fMin);
}

Asset * getBoxAsset()
{
  std::vector<Vertex> vertices;

  vertices.emplace_back(glm::vec3(1, 1, 0), glm::vec2(0, 0), glm::vec3(0, 0, -1));
  vertices.emplace_back(glm::vec3(-1, 1, 0), glm::vec2(0, 1), glm::vec3(0, 0, -1));
  vertices.emplace_back(glm::vec3(-1, -1, 0), glm::vec2(1, 1), glm::vec3(0, 0, -1));
  vertices.emplace_back(glm::vec3(1, -1, 0), glm::vec2(1, 0), glm::vec3(0, 0, -1));

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
  
  Asset * fighterAsset(new Asset());
  fighterAsset->setTexture(new Texture("./res/texture.jpg"));
  fighterAsset->setMesh(new Mesh(vertices, indices));
  return fighterAsset;
}


Asset * getSensorAsset()
{
  static Asset * s_asset = nullptr;
  if(s_asset != nullptr)
  {
    return s_asset;
  }
  std::vector<Vertex> vertices;

  vertices.emplace_back(glm::vec3(-1, -1, 0), glm::vec2(0, 0), glm::vec3(0, 0, -1));
  vertices.emplace_back(glm::vec3(1, -1, 0), glm::vec2(0, 0), glm::vec3(0, 0, -1));
  vertices.emplace_back(glm::vec3(2, 30, 0), glm::vec2(0, 1), glm::vec3(0, 0, -1));
  vertices.emplace_back(glm::vec3(-2, 30, 0), glm::vec2(1, 0), glm::vec3(0, 0, -1));

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
  
  Asset * fighterAsset(new Asset());
  fighterAsset->setTexture(new Texture("./res/texture.png"));
  fighterAsset->setMesh(new Mesh(vertices, indices));
  s_asset = fighterAsset;
  return fighterAsset;
}

void addObject(std::vector<Object*> & p_objects, Asset * p_asset, SpatialTree * tree)
{
  Object * object = new Object();
  BasicAi * ai = new BasicAi();
  ai->setObject(object);
  object->setAi(ai);
  
  double power = 4;

  double rand = fRand(0, 2 * 3.1415926);

  float cosCounter = cosf(rand);
  float sinCounter = sinf(rand);
  float rotspeed = 0;
/*if (p_objects.size() == 0)
  {
    rotspeed = 31.415926;
  }*/

  Sensor * objectSensor = new Sensor();
  
  objectSensor->setAngle(0);
  objectSensor->setPosition(glm::i64vec2(0, 0));
  objectSensor->addAsset(getSensorAsset());

  Component * objectComponent1 = new Component();
  
  objectComponent1->setAngle(0);
  objectComponent1->setPosition(glm::i64vec2(0, 0));
  objectComponent1->addAsset(p_asset);
  Component * objectComponent2 = new Component();
  
  objectComponent2->setAngle(3.1415912/4);
  objectComponent2->setPosition(glm::i64vec2(0, OBJTOWORLD*0.5));
  objectComponent2->addAsset(p_asset);

  object->addComponent(objectComponent1);
  object->addComponent(objectComponent2);
  object->addComponent(objectSensor);
  object->setRot(0);
  object->setXPos(cosCounter * OBJTOWORLD * p_objects.size() * 4 + OBJTOWORLD * 30);
  object->setYPos(sinCounter * OBJTOWORLD * p_objects.size() * 4 + OBJTOWORLD * 30);
  object->updateTransform();
  object->setHalfSize(glm::u64vec2(OBJTOWORLD, OBJTOWORLD));
  object->setRotSpeed(1);
//  object->setSpeed(glm::i64vec2(-cosCounter * OBJTOWORLD * std::sqrt(p_objects.size()*2), -sinCounter * OBJTOWORLD * std::sqrt(p_objects.size()*2)));
  
  ai->addSensor(objectSensor);
  /**
  CircleShape * shape = new CircleShape;
  shape->setPos(glm::i64vec2(0, 0));
  shape->setRadius(OBJTOWORLD*2);
  /*/
  PolygonShape * sensorShape = new PolygonShape;
  std::vector<glm::f64vec2> vertices;
  vertices.resize(4);
  vertices[0] = glm::f64vec2(OBJTOWORLD * 2, OBJTOWORLD * 30);
  vertices[1] = glm::f64vec2(OBJTOWORLD, -OBJTOWORLD);
  vertices[2] = glm::f64vec2(-OBJTOWORLD, -OBJTOWORLD);
  vertices[3] = glm::f64vec2(-OBJTOWORLD * 2, OBJTOWORLD * 30);
  sensorShape->setVertices(vertices);

  Fixture * sensorFixture(new Fixture);
  sensorFixture->density = 0;
  sensorFixture->friction = 0;
  sensorFixture->restitution = 0;
  sensorFixture->object = object;
  sensorFixture->shape = sensorShape;
  sensorFixture->type = Fixture::eSensor;
  
  objectSensor->addSensorFixture(sensorFixture);

  
  PolygonShape * shape1 = new PolygonShape;
  vertices.resize(4);
  vertices[0] = glm::f64vec2(OBJTOWORLD, OBJTOWORLD);
  vertices[1] = glm::f64vec2(OBJTOWORLD, -OBJTOWORLD);
  vertices[2] = glm::f64vec2(-OBJTOWORLD, -OBJTOWORLD);
  vertices[3] = glm::f64vec2(-OBJTOWORLD, OBJTOWORLD);

  shape1->setVertices(vertices);
  //*/
  Fixture * fixture(new Fixture);
  fixture->density = 0.0000000000000000000001;
  fixture->friction = 0.2;
  fixture->restitution = 1;
  fixture->object = object;
  fixture->shape = shape1;
  fixture->type = Fixture::eNormal;
  
  objectComponent1->addFixture(fixture);

  PolygonShape * shape2 = new PolygonShape;
  vertices.resize(4);
  vertices[0] = glm::f64vec2(OBJTOWORLD, OBJTOWORLD);
  vertices[1] = glm::f64vec2(OBJTOWORLD, -OBJTOWORLD);
  vertices[2] = glm::f64vec2(-OBJTOWORLD, -OBJTOWORLD);
  vertices[3] = glm::f64vec2(-OBJTOWORLD, OBJTOWORLD);
  shape2->setVertices(vertices);
  
  Fixture * fixture2(new Fixture);
  fixture2->density = 0.0000000000000000000001;
  fixture2->friction = 0.2;
  fixture2->restitution = 1;
  fixture2->object = object;
  fixture2->shape = shape2;
  fixture2->type = Fixture::eNormal;
  
  objectComponent2->addFixture(fixture2);
  object->updateMass();

  tree->addObject(object);
  p_objects.push_back(object);
}

void removeObject(std::vector<Object*> & p_objects)
{
  delete p_objects.back();
  p_objects.pop_back();
}

void generateTestObjects(GameState & p_state)
{
  Asset * asset(getBoxAsset());
  for(int index = 0;
  index < 2000;
    index++)
  {
    addObject(p_state.objects, asset, p_state.spatialTree);
    p_state.spatialTree = p_state.spatialTree->top();
  }
}

int main(int argc, char ** argv)
{
  SDL_Init(SDL_INIT_EVERYTHING);

  Display display(1600, 800, "Screen 1");
  CameraWorldBased camera(glm::i64vec2(0, 0), glm::i32vec2(1600, 800), OBJTOWORLD / 16, 1);
  camera.updateTransform();
  display.setCamera(&camera);

  AABB aabb;
  aabb.setCenter(glm::i64vec2(0, 0));
  aabb.setSize(glm::u64vec2(std::numeric_limits<glm::u64>::max() / 16, std::numeric_limits<glm::u64>::max() / 16));

  GameState state;
  state.focusedObject = nullptr;
  state.displays.push_back(&display);
  state.spatialTree = new QuadTree(aabb);
  state.spatialTree = state.spatialTree->top();
  state.prevFrameTime = SDL_GetTicks();
  state.currentFrameTime = SDL_GetTicks();

  generateTestObjects(state);

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

  SDL_Quit();

  return 0;
}
