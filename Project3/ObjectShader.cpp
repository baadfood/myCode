/*#include "ObjectShader.h"

#include <fstream>
#include <iostream>

#include <GL/glew.h>

#include "CameraWorldBased.h"

#include "LightSource.h"

namespace
{
  enum EShaderType
  {
    eVertexShader,
    eFragmentShader,
    eNumShaders
  };
}

struct ObjectShader::Private
{
  GLuint program;
  GLuint shaders[eNumShaders];
  GLuint uniforms[eNumUniforms];
};

ObjectShader::ObjectShader(std::string const & p_filename)
{
  d = new Private();
  d->program = glCreateProgram();

  d->shaders[eVertexShader] = createShader(loadFile(p_filename + ".vs"), GL_VERTEX_SHADER);
  d->shaders[eFragmentShader] = createShader(loadFile(p_filename + ".fs"), GL_FRAGMENT_SHADER);

  for (unsigned int index = 0; index < eNumShaders; index++)
  {
    glAttachShader(d->program, d->shaders[index]);
  }

  glBindAttribLocation(d->program, 0, "position");
  glBindAttribLocation(d->program, 1, "texCoord");
  glBindAttribLocation(d->program, 2, "normal");

  glLinkProgram(d->program);
  checkProgramError(d->program, GL_LINK_STATUS, std::string("Shader program link error ") + p_filename);

  glValidateProgram(d->program);
  checkProgramError(d->program, GL_LINK_STATUS, std::string("Shader program validation error ") + p_filename);

  d->uniforms[eCamera] = glGetUniformLocation(d->program, "camera");
  d->uniforms[eCameraPosition] = glGetUniformLocation(d->program, "cameraPosition");
  d->uniforms[eLightPos] = glGetUniformLocation(d->program, "light.position");
  d->uniforms[eLightIntensities] = glGetUniformLocation(d->program, "light.intensities");
  d->uniforms[eLightAttenuation] = glGetUniformLocation(d->program, "light.attenuation");
  d->uniforms[eLightAmbientCoefficient] = glGetUniformLocation(d->program, "light.ambientCoefficient");
  d->uniforms[eExposure] = glGetUniformLocation(d->program, "exposure");
  d->uniforms[eLightDistanceModifier] = glGetUniformLocation(d->program, "lightDistanceModifier");
}

GLuint ObjectShader::getProgram() const
{
  return d->program;
}

ObjectShader::~ObjectShader()
{
  for (unsigned int index = 0; index < eNumShaders; index++)
  {
    glDetachShader(d->program, d->shaders[index]);
    glDeleteShader(d->shaders[index]);
  }

  glDeleteProgram(d->program);
  delete d;
}

void ObjectShader::bind()
{
  glUseProgram(d->program);
}

void ObjectShader::update(std::vector<Object*> const & p_objects, CameraWorldBased const * p_camera, LightSource const & p_light)
{
  updateCamera(p_camera);
  updateObjects(p_objects);
  updateLightSource(p_light);
}

void ObjectShader::updateCamera(CameraWorldBased const * p_camera)
{
  glm::vec3 cameraPos(0, 0, -5);
  GLfloat distanceMod = p_camera->getWorldPerPixel();
  glUniformMatrix4fv(d->uniforms[eCamera], 1, GL_FALSE, &(p_camera->getTransform()[0][0]));
  glUniform3fv(d->uniforms[eCameraPosition], 1, &cameraPos[0]);
  glUniform1fv(d->uniforms[eExposure], 1, &p_camera->getExposure());
  glUniform1fv(d->uniforms[eLightDistanceModifier], 1, &distanceMod);
}

void ObjectShader::updateLightSource(LightSource const & p_light)
{
  glUniform3fv(d->uniforms[eLightIntensities], 1, &p_light.intensities[0]);
  glUniform3fv(d->uniforms[eLightPos], 1, &p_light.position[0]);
  glUniform1fv(d->uniforms[eLightAttenuation], 1, &p_light.attenuation);
  glUniform1fv(d->uniforms[eLightAmbientCoefficient], 1, &p_light.ambienCoefficient);
}

void ObjectShader::updateObjects(std::vector<Object*> const & p_objects)
{

}

// void ObjectShader::updateObjectPos(Object const & p_object)
// {
//   glUniformMatrix4fv(d->uniforms[eTransform], 1, GL_FALSE, &(p_object.getTransform()[0][0]));
// }
// void ObjectShader::updateObjectPos(Transform const & p_transform)
// {
//   glUniformMatrix4fv(d->uniforms[eTransform], 1, GL_FALSE, &(p_transform.getModel()[0][0]));
// }

std::string ObjectShader::loadFile(std::string const & p_filename)
{
  std::ifstream file;
  file.open(p_filename.c_str());

  std::string retval;
  std::string line;

  if (file.is_open() == false)
  {
    std::cerr << "Failed to open file " << p_filename << " For reading\n";
    return retval;
  }

  while (file.good())
  {
    std::getline(file, line);
    retval.append(line + "\n");
  }
  if (retval.empty())
  {
    std::cerr << "Shader file was empty " << p_filename << std::endl;
  }

  return retval;
}

GLuint ObjectShader::createShader(std::string const & p_source, GLenum p_shaderType)
{
  GLuint shader = glCreateShader(p_shaderType);

  if (shader == 0)
  {
    std::cerr << "Shader creation failed\n";
    return 0;
  }

  const GLchar * sources[1];
  GLint sourceLengths[1];

  sources[0] = p_source.c_str();
  sourceLengths[0] = static_cast<int>(p_source.length());

  glShaderSource(shader, 1, sources, sourceLengths);

  glCompileShader(shader);

  checkShaderError(shader, GL_COMPILE_STATUS, "Shader compile failed");

  return shader;
}


void ObjectShader::checkShaderError(GLuint p_shader, GLuint p_flag, const std::string & p_errorMessage)
{
  GLint success = 0;
  GLchar error[1024] = { 0 };

  glGetShaderiv(p_shader, p_flag, &success);
  if (success == GL_FALSE)
  {
    glGetShaderInfoLog(p_shader, sizeof(error), NULL, error);

    std::cerr << p_errorMessage << ": '" << error << "'" << std::endl;
  }
}

void ObjectShader::checkProgramError(GLuint p_program, GLuint p_flag, const std::string & p_errorMessage)
{
  GLint success = 0;
  GLchar error[1024] = { 0 };

  glGetProgramiv(p_program, p_flag, &success);
  if (success == GL_FALSE)
  {
    glGetProgramInfoLog(p_program, sizeof(error), NULL, error);

    std::cerr << p_errorMessage << ": '" << error << "'" << std::endl;
  }
}
*/