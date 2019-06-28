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

#include "Components/Sensor.h"
#include "Components/Engine.h"

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

Asset* getEngineAsset()
{
	std::vector<Vertex> vertices;

	vertices.emplace_back(glm::vec3(0.25, 0.5, 0), glm::vec2(0, 0), glm::vec3(0, 0, -1));
	vertices.emplace_back(glm::vec3(-0.25, 0.5, 0), glm::vec2(0, 1), glm::vec3(0, 0, -1));
	vertices.emplace_back(glm::vec3(-0.5, -0.5, 0), glm::vec2(1, 1), glm::vec3(0, 0, -1));
	vertices.emplace_back(glm::vec3(0.5, -0.5, 0), glm::vec2(1, 0), glm::vec3(0, 0, -1));

	unsigned int indics[] = { 0, 1, 2,
	  0, 2, 3
	};
	std::vector<unsigned int> indices;
	for (int index = 0;
		index < sizeof(indics) / sizeof(unsigned int);
		index++)
	{
		indices.push_back(indics[index]);
	}

	Asset* fighterAsset(new Asset());
	fighterAsset->setTexture(new Texture("./res/sensor.png"));
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

  vertices.emplace_back(glm::vec3(0, 0, 0), glm::vec2(0, 0), glm::vec3(0, 0, -1));
  vertices.emplace_back(glm::vec3(12, 40, 0), glm::vec2(0, 1), glm::vec3(0, 0, -1));
  vertices.emplace_back(glm::vec3(0, 41, 0), glm::vec2(1, 1), glm::vec3(0, 0, -1));
  vertices.emplace_back(glm::vec3(-12, 40, 0), glm::vec2(1, 0), glm::vec3(0, 0, -1));

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
  fighterAsset->setTexture(new Texture("./res/sensor.png"));
  fighterAsset->setMesh(new Mesh(vertices, indices));
  s_asset = fighterAsset;
  return fighterAsset;
}

void addObject(std::vector<Object*> & p_objects, Asset * p_asset, SpatialTree * tree)
{
  Object * object = new Object(std::to_string(p_objects.size() + 1));
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


  Component * objectComponent1 = new Component();
  
  objectComponent1->setAngle(0);
  objectComponent1->setPosition(glm::i64vec2(0, 0));
  objectComponent1->addAsset(p_asset);

  object->addComponent(objectComponent1);
//   object->addComponent(objectSensor);
  object->setRot(0);
  object->setXPos(cosCounter * OBJTOWORLD * p_objects.size() * 4 + OBJTOWORLD * 30);
  object->setYPos(sinCounter * OBJTOWORLD * p_objects.size() * 4 + OBJTOWORLD * 30);
  object->updateTransform();
  object->setHalfSize(glm::u64vec2(OBJTOWORLD, OBJTOWORLD));
  object->setRotSpeed(0);
  object->setSpeed(glm::i64vec2(-cosCounter * OBJTOWORLD * std::sqrt(p_objects.size()), -sinCounter *OBJTOWORLD * std::sqrt(p_objects.size())));
  std::vector<glm::f64vec2> vertices;
  
//   ai->addSensor(objectSensor);
  /**
  CircleShape * shape = new CircleShape;
  shape->setPos(glm::i64vec2(0, 0));
  shape->setRadius(OBJTOWORLD*2);
  /*/
//   PolygonShape * sensorShape = new PolygonShape;
//   std::vector<glm::f64vec2> vertices;
//   vertices.resize(4);
//   vertices[0] = glm::f64vec2(0, 0);
//   vertices[1] = glm::f64vec2(-OBJTOWORLD, OBJTOWORLD*10);
//   vertices[2] = glm::f64vec2(0, OBJTOWORLD*11);
//   vertices[3] = glm::f64vec2(OBJTOWORLD, OBJTOWORLD*10);
//   sensorShape->setVertices(vertices);

//   Fixture * sensorFixture(new Fixture);
//   sensorFixture->density = 0;
//   sensorFixture->friction = 0;
//   sensorFixture->restitution = 0;
//   sensorFixture->object = object;
//   sensorFixture->shape = sensorShape;
//   sensorFixture->type = Fixture::eSensor;
//   
//   objectSensor->addSensorFixture(sensorFixture);

  
  PolygonShape * shape1 = new PolygonShape;
  vertices.resize(4);
  vertices[0] = glm::f64vec2(OBJTOWORLD, OBJTOWORLD);
  vertices[1] = glm::f64vec2(OBJTOWORLD, -OBJTOWORLD);
  vertices[2] = glm::f64vec2(-OBJTOWORLD, -OBJTOWORLD);
  vertices[3] = glm::f64vec2(-OBJTOWORLD, OBJTOWORLD);

  shape1->setVertices(vertices);
  //*/
  Fixture * fixture(new Fixture);
  fixture->density = 0.1;
  fixture->friction = 1;
  fixture->restitution = 1;
  fixture->object = object;
  fixture->shape = shape1;
  fixture->type = Fixture::eNormal;
  
  objectComponent1->addFixture(fixture);

  object->updateMass();

  tree->addObject(object);
  p_objects.push_back(object);
}

void addMissile(std::vector<Object*> & p_objects, Asset * p_asset, SpatialTree * tree)
{
  Object * object = new Object(std::to_string(p_objects.size() + 1));
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

  Component * reticule = new Component();
  reticule->setAngle(0);
  reticule->setPosition(glm::i64vec2(0, 0));
  reticule->addAsset(p_asset);
  ai->setTargettingReticule(reticule);
  
  Component * objectComponent1 = new Component();
  
  objectComponent1->setAngle(0);
  objectComponent1->setPosition(glm::i64vec2(0, 0));
  objectComponent1->addAsset(p_asset);

  object->addComponent(objectComponent1);
  object->addComponent(objectSensor);
  object->addComponent(reticule);
  object->setRot(0);
  object->setXPos(cosCounter * OBJTOWORLD * p_objects.size() + OBJTOWORLD * 30);
  object->setYPos(sinCounter * OBJTOWORLD * p_objects.size() + OBJTOWORLD * 30);
//  object->setXPos(-OBJTOWORLD*12);
//  object->setYPos(OBJTOWORLD*3);
  object->updateTransform();
  object->setHalfSize(glm::u64vec2(OBJTOWORLD/4, OBJTOWORLD/4));
  object->setRotSpeed(1);
  object->setSpeed(glm::i64vec2(OBJTOWORLD * 0.4, 0));
//  object->setSpeed(glm::i64vec2(-cosCounter * OBJTOWORLD * std::sqrt(p_objects.size()*2), -sinCounter * OBJTOWORLD * std::sqrt(p_objects.size()*2)));
  
  ai->addSensor(objectSensor);
  
  Engine * forwardEngine = new Engine(OBJTOWORLD/1000);
  forwardEngine->setAngle(0);
  forwardEngine->setPosition(glm::i64vec2(0, -OBJTOWORLD));
//  forwardEngine->addAsset(getEngineAsset());

//   Engine * clockwiseEngine1 = new Engine(OBJTOWORLD/100);
//   clockwiseEngine1->setAngle(3.1415912/2);
//   clockwiseEngine1->setPosition(glm::i64vec2(0, -OBJTOWORLD));

  Engine * clockwiseEngine2 = new Engine(OBJTOWORLD/100000);
  clockwiseEngine2->setAngle(-3.1415912/2);
  clockwiseEngine2->setPosition(glm::i64vec2(0, OBJTOWORLD));
//  clockwiseEngine2->addAsset(getEngineAsset());

//   Engine * cClockwiseEngine1 = new Engine(OBJTOWORLD/100);
//   cClockwiseEngine1->setAngle(-3.1415912/2);
//   cClockwiseEngine1->setPosition(glm::i64vec2(0, -OBJTOWORLD));

  Engine * cClockwiseEngine2 = new Engine(OBJTOWORLD/100000);
  cClockwiseEngine2->setAngle(3.1415912/2);
  cClockwiseEngine2->setPosition(glm::i64vec2(0, OBJTOWORLD));
//  cClockwiseEngine2->addAsset(getEngineAsset());

  
  object->addComponent(forwardEngine);
//   object->addComponent(clockwiseEngine1);
//   object->addComponent(cClockwiseEngine1);
  object->addComponent(clockwiseEngine2);
  object->addComponent(cClockwiseEngine2);
  ai->addEngine(forwardEngine, BasicAi::eForward);
//   ai->addEngine(clockwiseEngine1, BasicAi::eClockwiseTurn);
//   ai->addEngine(cClockwiseEngine1, BasicAi::eCounterCTurn);
  ai->addEngine(clockwiseEngine2, BasicAi::eClockwiseTurn);
  ai->addEngine(cClockwiseEngine2, BasicAi::eCounterCTurn);

  
  /**
  CircleShape * shape = new CircleShape;
  shape->setPos(glm::i64vec2(0, 0));
  shape->setRadius(OBJTOWORLD*2);
  /*/
  PolygonShape * sensorShape = new PolygonShape;
  std::vector<glm::f64vec2> vertices;
  vertices.resize(4);
  vertices[0] = glm::f64vec2(0, 0);
  vertices[1] = glm::f64vec2(-OBJTOWORLD*22, OBJTOWORLD*40);
  vertices[2] = glm::f64vec2(0, OBJTOWORLD*41);
  vertices[3] = glm::f64vec2(OBJTOWORLD*22, OBJTOWORLD*40);
  sensorShape->setVertices(vertices);

  Fixture * sensorFixture(new Fixture);
  sensorFixture->density = 0;
  sensorFixture->friction = 0;
  sensorFixture->restitution = 0;
  sensorFixture->object = object;
  sensorFixture->shape = sensorShape;
  sensorFixture->type = Fixture::eSensor;
  
  objectSensor->addSensorFixture(sensorFixture);

  PolygonShape * reticuleShape = new PolygonShape;
  vertices.resize(4);
  vertices[0] = glm::f64vec2(OBJTOWORLD, OBJTOWORLD);
  vertices[1] = glm::f64vec2(OBJTOWORLD, -OBJTOWORLD);
  vertices[2] = glm::f64vec2(-OBJTOWORLD, -OBJTOWORLD);
  vertices[3] = glm::f64vec2(-OBJTOWORLD, OBJTOWORLD);

  reticuleShape->setVertices(vertices);
  //*/
  Fixture * retFixture(new Fixture);
  retFixture->density = 0.0;
  retFixture->friction = 0.0;
  retFixture->restitution = 0.0;
  retFixture->object = object;
  retFixture->shape = reticuleShape;
  retFixture->type = Fixture::eSensor;
  
  reticule->addFixture(retFixture);
  
  PolygonShape * shape1 = new PolygonShape;
  vertices.resize(4);
  vertices[0] = glm::f64vec2(OBJTOWORLD/4, OBJTOWORLD/4);
  vertices[1] = glm::f64vec2(OBJTOWORLD/4, -OBJTOWORLD/4);
  vertices[2] = glm::f64vec2(-OBJTOWORLD/4, -OBJTOWORLD/4);
  vertices[3] = glm::f64vec2(-OBJTOWORLD/4, OBJTOWORLD/4);
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

  PolygonShape* engine1Shape = new PolygonShape;
  vertices.resize(4);
  vertices[0] = glm::f64vec2(OBJTOWORLD / 16, OBJTOWORLD / 8);
  vertices[1] = glm::f64vec2(OBJTOWORLD / 8, -OBJTOWORLD / 8);
  vertices[2] = glm::f64vec2(-OBJTOWORLD / 8, -OBJTOWORLD / 8);
  vertices[3] = glm::f64vec2(-OBJTOWORLD / 16, OBJTOWORLD / 8);
  engine1Shape->setVertices(vertices);

  Fixture* engine1Fix(new Fixture);
  engine1Fix->density = 0;
  engine1Fix->friction = 0;
  engine1Fix->restitution = 0;
  engine1Fix->object = object;
  engine1Fix->shape = engine1Shape;
  engine1Fix->type = Fixture::eSensor;
  forwardEngine->addFixture(engine1Fix);

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
  index < 50000;
    index++)
  {
    addObject(p_state.objects, asset, p_state.spatialTree);
//    addMissile(p_state.objects, asset, p_state.spatialTree);
    p_state.spatialTree = p_state.spatialTree->top();
  }
}

int main(int argc, char ** argv)
{
  SDL_Init(SDL_INIT_EVERYTHING);

  Display display(1920, 1024, "Screen 1");
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

  while(display.isClosed() == false)
  {
    if (frames++ == 1000)
    {
      break;
    }
    manager.progressFrame();
  }

  unsigned int lastTicks = SDL_GetTicks();

  std::cout << lastTicks - tickStart << '\n';

  SDL_Quit();

  return 0;
}
