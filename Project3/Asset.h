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

  virtual std::shared_ptr<Mesh> getMesh();
  virtual std::shared_ptr<Texture> getTexture();

  virtual void setMesh(std::shared_ptr<Mesh> p_mesh);
  virtual void setTexture(std::shared_ptr<Texture> p_texture);

protected:
private:
  struct Private;
  std::unique_ptr<Private> d;
};

