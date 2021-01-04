#include "Mesh.h"

#ifdef __APPLE__
  #include <OpenGL/gl.h>
#else
  #include <GL/glew.h>
#endif
#include "Vertex.h"
#include <iostream>

#include "obj_loader.h"
#include "Shader.h"

struct Mesh::Private
{
  unsigned int drawCount;
  GLuint positionVb;
  GLuint normalVb;
  GLuint texCoordVb;

  GLuint buffers[Mesh::eNumBuffers];

  Shader * prevShader;
};

Mesh::Mesh()
{
  d = new Private();
}

Mesh::Mesh(std::string const & p_filename)
{
  d = new Private();
  IndexedModel model = OBJModel(p_filename).ToIndexedModel();
  initMesh(model);
}

unsigned int Mesh::getDrawCount() const
{
  return d->drawCount;
}

void Mesh::initMesh(IndexedModel const & p_model)
{
  d->drawCount = static_cast<unsigned int>(p_model.indices.size());
  glGenBuffers(Mesh::eNumBuffers, d->buffers);

  glBindBuffer(GL_ARRAY_BUFFER, d->buffers[ePosition_Vb]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(p_model.positions[0]) * p_model.positions.size(), p_model.positions.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, d->buffers[eTexCoord_Vb]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(p_model.texCoords[0]) * p_model.texCoords.size(), p_model.texCoords.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, d->buffers[eNormal_Vb]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(p_model.normals[0]) * p_model.normals.size(), p_model.normals.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d->buffers[eIndex_Vb]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * p_model.indices.size(), p_model.indices.data(), GL_STATIC_DRAW);
}

Mesh::Mesh(std::vector<Vertex> const & p_vertices, std::vector<unsigned int> p_indices)
{
  d = new Private();

  IndexedModel model;

  model.positions.reserve(p_vertices.size());
  model.texCoords.reserve(p_vertices.size());

  for (int index = 0; index < p_vertices.size(); index++)
  {
    model.positions.push_back(p_vertices[index].getPos());
    model.texCoords.push_back(p_vertices[index].getTexCoord());
    model.normals.push_back(p_vertices[index].getNormal());
  }

  model.indices = p_indices;

  initMesh(model);
}

Mesh::~Mesh()
{
  glDeleteBuffers(Mesh::eNumBuffers, d->buffers);
  delete d;
}

void Mesh::bind(Shader const * p_shader)
{
/*  if(d->prevShader != p_shader)
  {
    // Not the same shader as last time, need to update attribute numbers
    Shader::Attribute attribute;
    if(p_shader->getAttribute("position", attribute) == false)
    {
      std::cerr << "'position' attribute not found in shader, cannot bind mesh\n";
      return;
    }
    d->positionVb = attribute.id;
    if(p_shader->getAttribute("normal", attribute) == false)
    {
      std::cerr << "'normal' attribute not found in shader, cannot bind mesh\n";
      return;
    }
    d->normalVb = attribute.id;
    Shader::Uniform uniform;
    if(p_shader->getUniform("tex", uniform) == false)
    {
      std::cerr << "'tex' uniform not found in shader, cannot bind mesh\n";
      return;
    }
    d->texCoordVb = uniform.id;
  }*/

  glBindBuffer(GL_ARRAY_BUFFER, d->buffers[ePosition_Vb]);
  glEnableVertexAttribArray(Shader::ePosition);
  glVertexAttribPointer(Shader::ePosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, d->buffers[eTexCoord_Vb]);
  glEnableVertexAttribArray(Shader::eTexCoord);
  glVertexAttribPointer(Shader::eTexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, d->buffers[eNormal_Vb]);
  glEnableVertexAttribArray(Shader::eNormal);
  glVertexAttribPointer(Shader::eNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d->buffers[eIndex_Vb]);
}

void Mesh::unbind()
{
  glBindBuffer(GL_ARRAY_BUFFER, d->buffers[ePosition_Vb]);
  glDisableVertexAttribArray(Shader::ePosition);

  glBindBuffer(GL_ARRAY_BUFFER, d->buffers[eTexCoord_Vb]);
  glDisableVertexAttribArray(Shader::eTexCoord);

  glBindBuffer(GL_ARRAY_BUFFER, d->buffers[eNormal_Vb]);
  glDisableVertexAttribArray(Shader::eNormal);
}