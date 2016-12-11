#include "Display.h"

#include <iostream>

#include <SDL.h>
#include <GL/glew.h>
#include "CameraBase.h"

#include "GameState.h"
#include "QuadTree.h"
#include "Object.h"
#include "utils.h"

bool objectPriority(glm::i64vec2 p_pos, Object const * p_first, Object const * p_second)
{
  return p_first->getAABB().getArea() * distanceSquared(p_pos, p_first->getPos()) < p_second->getAABB().getArea() * distanceSquared(p_pos, p_second->getPos());
}

struct Display::Private 
{
  SDL_Window * window;
  SDL_GLContext glContext;
  bool quit;
  Uint32 windowId;
  CameraBase * camera;
  Object * inputFocus;
  std::vector<UserInputHandler *> screenInputHandlers;
};

Display::Display(int p_width, int p_height, std::string const & p_title)
{
  d = new Private();

  d->quit = false;

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

  d->window = SDL_CreateWindow(p_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, p_width, p_height, SDL_WINDOW_OPENGL);
  d->glContext = SDL_GL_CreateContext(d->window);

  GLenum status = glewInit();

  if (status != GLEW_OK)
  {
    std::cerr << "GLEW failed to initialize\n";
  }
  SDL_GL_SetSwapInterval(0);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  d->windowId = SDL_GetWindowID(d->window);
}


Display::~Display()
{
  SDL_GL_DeleteContext(d->glContext);
  SDL_DestroyWindow(d->window);
  delete d;
}

Uint32 Display::getWinId()
{
  return d->windowId;
}

CameraBase * Display::getCamera()
{
  return d->camera;
}

void Display::setCamera(CameraBase * p_camera)
{
  d->camera = p_camera;
}

void Display::clear(float p_red, float p_green, float p_blue, float p_alpha)
{
  glClearColor(p_red, p_green, p_blue, p_alpha);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Display::update()
{
  SDL_GL_SwapWindow(d->window);
}

bool Display::isClosed()
{
  return d->quit;
}

bool Display::handleEvent(SDL_Event * p_event, GameState * p_state)
{
  // Handle focus changing events.
  if(p_event->type == SDL_MOUSEBUTTONDOWN
     &&  p_event->button.button == 1)
  {
    glm::i64vec2 worldPos = d->camera->screenToWorld(glm::i32vec2(p_event->motion.x, p_event->motion.y));
    std::cout << "clicked at X:" << worldPos.x << " Y:" << worldPos.y << std::endl;
    std::vector<Object *> objects;
    p_state->spatialTree->getObjectsAt(worldPos, objects);
    if(d->inputFocus)
    {
      d->inputFocus->handleLostFocus(p_event);
    }
    d->inputFocus = nullptr;

    auto sortFunc = std::bind(objectPriority, worldPos, std::placeholders::_1, std::placeholders::_2);

    std::sort(objects.begin(), objects.end(), sortFunc);

    for(auto iter = objects.begin();
    iter != objects.end();
      iter++)
    {
      if((*iter)->handleGotFocus(p_event))
      {
        d->inputFocus = (*iter);
        break;
      }
    }
  }

  if(d->inputFocus != nullptr
  && d->inputFocus->handleInput(p_event))
  {
    // Yey, object handled and consumed event, nothing to do here but skip rest of the handlers.
    return true;
  }

  bool gotPos = true;
  int screenX;
  int screenY;
  switch(p_event->type)
  {
    case SDL_MOUSEBUTTONDOWN:
    screenX = p_event->button.x;
    screenY = p_event->button.y;
    break;
    case SDL_MOUSEMOTION:
    screenX = p_event->button.x;
    screenY = p_event->button.y;
    break;
    case SDL_MOUSEBUTTONUP:
    screenX = p_event->button.x;
    screenY = p_event->button.y;
    break;
    case SDL_MOUSEWHEEL:
    screenX = p_event->button.x;
    screenY = p_event->button.y;
    break;
    default:
    gotPos = false;
    break;
  }
  if(gotPos)
  {
    glm::i64vec2 worldPos = d->camera->screenToWorld(glm::i32vec2(screenX, screenY));
    std::vector<Object *> objects;
    p_state->spatialTree->getObjectsAt(worldPos, objects);
    auto sortFunc = std::bind(objectPriority, worldPos, std::placeholders::_1, std::placeholders::_2);

    static size_t nobjects = 0;
    
    if(objects.size() != nobjects)
    {
      std::cout << "Objects under cursor: " << objects.size() << std::endl;
      nobjects = objects.size();
    }

    std::sort(objects.begin(), objects.end(), sortFunc);

    for(auto iter = objects.begin();
    iter != objects.end();
      iter++)
    {
      if((*iter)->handleInput(p_event))
      {
        return true;
      }
    }
  }
  // Handle global keyboard shortcuts for this screen.
  for(auto iter = d->screenInputHandlers.begin();
  iter != d->screenInputHandlers.end();
    iter++)
  {
    if((*iter)->handleInput(p_event))
    {
      // Input was consumed.
      return true;
    }
  }

  if(d->camera->handleInput(p_event))
  {
    return true;
  }

  return false;
}
