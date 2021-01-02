#ifndef Mesh_h_
#define Mesh_h_

#include <vector>
#include "Vertex.h"
#include "obj_loader.h"
#include "Object.h"

#ifdef __APPLE__
  #include <OpenGL/glext.h>
#else
  #include <GL/glew.h>
#endif
class Shader;

class Mesh
{
public:
  Mesh();
  Mesh(std::vector<Vertex> const & p_vertices, std::vector<unsigned int> p_indices);
  Mesh(std::string const & p_filename);
  virtual ~Mesh();

  virtual unsigned int getDrawCount() const;

  void bind(Shader const * p_shader);
  void unbind();

  enum EBuffers
  {
    ePosition_Vb,
    eTexCoord_Vb,
    eIndex_Vb,
    eNormal_Vb,
    eNumBuffers
  };

private:
  void initMesh(IndexedModel const & p_model);

  Mesh(Mesh const & p_other) {}
  Mesh const & operator=(Mesh const & p_other) {}

  struct Private;
  Private * d;
};

#endif