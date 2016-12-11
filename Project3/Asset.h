#pragma once

#include "Base.h"

#include <memory>

class Mesh;
class Texture;
class Shader;

class Asset: public Base
{
public:
  Asset();
  virtual ~Asset();

  virtual void bind(Shader const * p_shader);
  virtual void unbind();

  virtual Mesh * getMesh();
  virtual Texture * getTexture();

  virtual void setMesh(Mesh * p_mesh);
  virtual void setTexture(Texture * p_texture);

protected:
private:
  struct Private;
  std::unique_ptr<Private> d;
};

