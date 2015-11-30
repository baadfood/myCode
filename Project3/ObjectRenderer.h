#pragma once
#include "Base.h"

#include <vector>
#include "Object.h"

class Shader;
class CameraBase;
struct GameState;

class ObjectRenderer:
  public Base
{
public:
  virtual void setShader(Shader * p_shader) = 0;
  virtual void setCamera(CameraWorldBased * p_camera) = 0;
  virtual void prepareRenderData(std::vector<Object*> p_objects, GameState * p_state) = 0;
  virtual void render() = 0;
};

