#ifndef Shader_h_
#define Shader_h_

#include <string>
#ifdef __APPLE__
  #include <OpenGL/glext.h>
#else
  #include <GL/glew.h>
#endif
#include "Transform.h"

class CameraBase;
struct LightSource;

class Shader
{
public:

  enum EAttributes
  {
    ePosition,
    eTexCoord,
    eNormal,
    eSelfIlluminationColor,
    eModel
  };

  struct Attribute
  {
  public:
    GLuint id;
    GLint size;
    GLenum type;
    std::string name;
  };

  struct Uniform
  {
  public:
    GLuint id;
    GLint size;
    GLenum type;
    std::string name;
  };

  Shader(std::string const & p_shaderPath);
  ~Shader();

  bool getAttribute(std::string const & p_name, Attribute & p_valueOut) const;
  bool getUniform(std::string const & p_name, Uniform & p_valueOut) const;
  GLuint getUniformLocation(char const * p_name);

  virtual void bind();
  virtual void unbind();

private:

  Shader(Shader const & p_other) {}
  Shader const & operator=(Shader const & p_other) {}

  std::string loadFile(std::string const & p_filename);

  GLuint createShader(std::string const & p_source, GLenum p_shaderType);

  void checkShaderError(GLuint p_shader, GLuint p_flag, const std::string & p_errorMessage);
  void checkProgramError(GLuint p_program, GLuint p_flag, const std::string & p_errorMessage);

  struct Private;
  Private * d;
};

#endif