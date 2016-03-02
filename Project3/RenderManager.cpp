#include "RenderManager.h"

#include "GameState.h"

#include "Display.h"
#include "CameraBase.h"
#include "CameraWorldBased.h"

#include "SpatialTree.h"

#include <unordered_map>
#include "ObjectRenderer.h"
#include "GenericRenderer.h"

#include "QuadTree.h"

struct RenderManager::Private
{
  std::unordered_map<glm::u32, std::unique_ptr<ObjectRenderer>> renderers;
  std::string name;
};

RenderManager::RenderManager() :
  d(new Private())
{
  d->name = "RenderManager";
  d->renderers[0] = std::unique_ptr<ObjectRenderer>(new GenericRenderer());
  d->renderers[0]->setShader(new Shader("./res/objectShader2"));
}

RenderManager::~RenderManager()
{
}

std::string const & RenderManager::getName() const
{
  return d->name;
}


void RenderManager::advance(GameState * p_state)
{
  // Render all dem displays, one display per camera currently.;

  // Maybe we can render all of the displays at the same time?;

  // Dunno, we won't have multiple display support quite yet;
  for(auto iter = p_state->displays.begin();
      iter != p_state->displays.end();
      iter++)
  {
    Display * currentDisplay = *iter;
    currentDisplay->clear(0.0, 0.0, 0.0, 1.0);
    CameraWorldBased * currentCamera = dynamic_cast<CameraWorldBased*>(currentDisplay->getCamera());

    std::vector<Object*> objectsToDraw;
    p_state->spatialTree->getObjectsIntersected(currentCamera->getAABB(), objectsToDraw);

    static int objectsInScreen = 0;
    if(objectsInScreen != objectsToDraw.size())
    {
      std::cout << "Objects in screen: " << objectsToDraw.size();
      objectsInScreen = objectsToDraw.size();
    }
    // group objects by renderer
    std::unordered_map<ObjectRenderer*, std::vector<Object*>> renderSetup;

    for(auto objectIter = objectsToDraw.begin();
    objectIter != objectsToDraw.end();
      objectIter++)
    {
      auto renderIter = d->renderers.find((*objectIter)->getTypeId());

      if(renderIter != d->renderers.end())
      {
        renderSetup[renderIter->second.get()].push_back(*objectIter);
      }
    }

    for(auto renderSetupIter = renderSetup.begin();
    renderSetupIter != renderSetup.end();
      renderSetupIter++)
    {
      renderSetupIter->first->setCamera(currentCamera);
      renderSetupIter->first->prepareRenderData(renderSetupIter->second, p_state);
    }

    for(auto renderSetupIter = renderSetup.begin();
    renderSetupIter != renderSetup.end();
      renderSetupIter++)
    {
      renderSetupIter->first->render();
    }
    currentDisplay->update();
  }
}