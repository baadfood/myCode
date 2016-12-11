#include "Asset.h"

#include "Texture.h"
#include "Mesh.h"
#include "Shader.h"

struct Asset::Private
{
  Mesh * mesh;
  Texture * texture;
};

Asset::Asset():
d(new Private())
{
}

Asset::~Asset()
{
}

void Asset::bind(Shader const * p_shader)
{
  d->texture->bind(0);
  d->mesh->bind(p_shader);
}

void Asset::unbind()
{
  d->texture->unbind();
  d->mesh->unbind();
}


Mesh * Asset::getMesh()
{
  return d->mesh;
}

Texture * Asset::getTexture()
{
  return d->texture;
}

void Asset::setMesh(Mesh * p_mesh)
{
  d->mesh = p_mesh;
}

void Asset::setTexture(Texture* p_texture)
{
  d->texture = p_texture;
}

