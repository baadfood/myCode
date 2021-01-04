#ifndef ObjectShader_h_
#define ObjectShader_h_

#include "Shader.h"

#include <string>
#ifdef __APPLE__
  #include <OpenGL/glext.h>
#else
  #include <GL/glew.h>
#endif
#include "Object.h"

#include "LightSource.h"

#include "Transform.h"

#include <vector>

class CameraWorldBased;

class ObjectShader: public Shader
{
public:
  ObjectShader(std::string const & p_shaderPath);
  ~ObjectShader();

  virtual void bind();
  virtual void update(std::vector<Object*> const & p_transform, CameraWorldBased const * p_camera, LightSource const & p_lightSource);
  void updateCamera(CameraWorldBased const * p_camera);
  void updateLightSource(LightSource const & p_light);
  void updateObjects(std::vector<Object*> const & p_objects);

  GLuint getProgram() const;

private:
  ObjectShader(ObjectShader const & p_other);
  ObjectShader const & operator=(ObjectShader const & p_other);

  std::string loadFile(std::string const & p_filename);

  GLuint createShader(std::string const & p_source, GLenum p_shaderType);

  void checkShaderError(GLuint p_shader, GLuint p_flag, const std::string & p_errorMessage);
  void checkProgramError(GLuint p_program, GLuint p_flag, const std::string & p_errorMessage);

  struct Private;
  Private * d;
};

#endif
