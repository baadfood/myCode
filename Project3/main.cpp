#include <iostream>
#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glext.h>
#else
  #include <GL/glew.h>
  #include <GL/gl.h>
#endif
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
#include "InputHandlers/FlyByWireAiController.inl"

#include "Components/Sensor.h"
#include "Components/Engine.h"

//FILE _iob[] = { *stdin, *stdout, *stderr };
#include <SDL.h>

#include <fstream>
#include "AI/FlyByWireAi.h"

double fRand(double fMin, double fMax)
{
  double f = (double)rand() / RAND_MAX;
  return fMin + f * (fMax - fMin);
}

Texture* getTexture()
{
  static Texture text("./res/texture.jpg");
  return &text;
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
  fighterAsset->setTexture(getTexture());
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

void addFlyByWireObject(std::vector<Object*>& p_objects, Asset* p_asset, SpatialTree* tree, Object* p_target, glm::i64vec2 p_offset, glm::f32 p_size = 1.0)
{
  Object* object = new Object(std::to_string(p_objects.size() + 1));
  FlyByWireAi * ai = new FlyByWireAi();
  if (p_target)
  {
    ai->setTarget(p_target);
    ai->setPositionOffset(p_offset);
  }
  else
  {
    ai->setDesiredLocation(glm::ivec2(0, 0));
  }

  auto controller = std::make_shared<FlyByWireAiController>(ai);
  ai->setObject(object);
  object->setAi(ai);
  object->addInputHandler(controller);

  Component* objectComponent1 = new Component();

  objectComponent1->setAngle(0);
  objectComponent1->setPosition(glm::i64vec2(0, 0));
  objectComponent1->addAsset(p_asset);

  object->addComponent(objectComponent1);
  object->setRot(0);
  object->setXPos(0);
  object->setYPos(0);

  object->updateTransform();
  object->setHalfSize(glm::u64vec2(OBJTOWORLD / 4 * p_size, OBJTOWORLD / 4 * p_size));
  object->setSpeed(glm::i64vec2(0, 0));

  std::vector<Engine*> engines;

  Engine* forwardEngine = new Engine(OBJTOWORLD / 10000);
  forwardEngine->setAngle(0);
  forwardEngine->setPosition(glm::i64vec2(0, -OBJTOWORLD / 3));
  engines.push_back(forwardEngine);

  Engine* backwardEngine = new Engine(OBJTOWORLD / 10000);
  backwardEngine->setAngle(M_PI);
  backwardEngine->setPosition(glm::i64vec2(0, OBJTOWORLD / 3));
  engines.push_back(backwardEngine);

  Engine* leftEngine = new Engine(OBJTOWORLD / 100000);
  leftEngine->setAngle(M_PI/2);
  leftEngine->setPosition(glm::i64vec2(OBJTOWORLD / 3, 0));
  engines.push_back(leftEngine);

  Engine* rightEngine = new Engine(OBJTOWORLD / 100000);
  rightEngine->setAngle(-M_PI/2);
  rightEngine->setPosition(glm::i64vec2(-OBJTOWORLD / 3, 0));
  engines.push_back(rightEngine);


  Engine * clockwiseEngine1 = new Engine(OBJTOWORLD / 100000);
  clockwiseEngine1->setAngle(3.1415912/2);
  clockwiseEngine1->setPosition(glm::i64vec2(OBJTOWORLD / 4 * p_size, -OBJTOWORLD / 5 * p_size));
  engines.push_back(clockwiseEngine1);

  //Engine* clockwiseEngine2 = new Engine(OBJTOWORLD / 10000000);
  //clockwiseEngine2->setAngle(-3.1415912 / 2);
  //clockwiseEngine2->setPosition(glm::i64vec2(-OBJTOWORLD / 4, OBJTOWORLD / 5));
  //engines.push_back(clockwiseEngine2);
  
  Engine * cClockwiseEngine1 = new Engine(OBJTOWORLD / 100000);
  cClockwiseEngine1->setAngle(-3.1415912/2);
  cClockwiseEngine1->setPosition(glm::i64vec2(-OBJTOWORLD / 4 * p_size, -OBJTOWORLD / 5 * p_size));
  engines.push_back(cClockwiseEngine1);

  //Engine* cClockwiseEngine2 = new Engine(OBJTOWORLD / 10000000);
  //cClockwiseEngine2->setAngle(3.1415912 / 2);
  //cClockwiseEngine2->setPosition(glm::i64vec2(OBJTOWORLD / 4, OBJTOWORLD / 5));
  //engines.push_back(cClockwiseEngine2);

/*  for (auto engine : engines)
  {
    engine->addAsset(getEngineAsset());
  }
  */

  object->addComponent(forwardEngine);
  object->addComponent(backwardEngine);
  object->addComponent(leftEngine);
  object->addComponent(rightEngine);
  object->addComponent(clockwiseEngine1);
  object->addComponent(cClockwiseEngine1);
//  object->addComponent(clockwiseEngine2);
//  object->addComponent(cClockwiseEngine2);
  ai->addEngine(forwardEngine, FlyByWireAi::eForward);
  ai->addEngine(backwardEngine, FlyByWireAi::eBackward);
  ai->addEngine(leftEngine, FlyByWireAi::eLeft);
  ai->addEngine(rightEngine, FlyByWireAi::eRight);
  ai->addEngine(clockwiseEngine1, FlyByWireAi::eClockwiseTurn);
  ai->addEngine(cClockwiseEngine1, FlyByWireAi::eCounterCTurn);
//  ai->addEngine(clockwiseEngine2, FlyByWireAi::eClockwiseTurn);
//  ai->addEngine(cClockwiseEngine2, FlyByWireAi::eCounterCTurn);

  std::vector<glm::f64vec2> vertices;

  PolygonShape* shape1 = new PolygonShape;
  vertices.resize(4);
  vertices[0] = glm::f64vec2(OBJTOWORLD / 4 * p_size, OBJTOWORLD / 4 * p_size);
  vertices[1] = glm::f64vec2(OBJTOWORLD / 4 * p_size, -OBJTOWORLD / 4 * p_size);
  vertices[2] = glm::f64vec2(-OBJTOWORLD / 4 * p_size, -OBJTOWORLD / 4 * p_size);
  vertices[3] = glm::f64vec2(-OBJTOWORLD / 4 * p_size, OBJTOWORLD / 4 * p_size);
  shape1->setVertices(vertices);
  //*/
  Fixture* fixture(new Fixture);
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
  index < 1;
    index++)
  {
    addObject(p_state.objects, asset, p_state.spatialTree);
    addMissile(p_state.objects, asset, p_state.spatialTree);
    p_state.spatialTree = p_state.spatialTree->top();
  }
  addFlyByWireObject(p_state.objects, asset, p_state.spatialTree, 0, glm::i64vec2(0,0), 3.0);
  Object* target = p_state.objects.back();
  const int c_formationSize = 1000;
  for (int index = -c_formationSize / 2;
       index < (c_formationSize / 2) + 1;
       index++)
  {
    if (index == 0) continue;
    addFlyByWireObject(p_state.objects, asset, p_state.spatialTree, target, glm::i64vec2(index % 50 * OBJTOWORLD + 3* OBJTOWORLD, std::abs(index % 50 + (index / 50)*2) * OBJTOWORLD+ 3 * OBJTOWORLD));
  }
}

int main(int argc, char ** argv)
{
  SDL_Init(SDL_INIT_EVERYTHING);

  Display display(1920, 1024, "Screen 1");
  CameraWorldBased camera(glm::i64vec2(0, 0), glm::i32vec2(1920, 1024), OBJTOWORLD / 16, 1);
  camera.updateTransform();
  display.setCamera(&camera);

  AABB aabb;
  aabb.setCenter(glm::i64vec2(0, 0));
  aabb.setSize(glm::u64vec2(std::numeric_limits<glm::u64>::max() / 2, std::numeric_limits<glm::u64>::max() / 2));

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

  while(display.isClosed() == false)
  {
    manager.progressFrame();
  }

  SDL_Quit();

  return 0;
}
