#include "Shader.h"

#include <fstream>
#include <iostream>

#include <GL/glew.h>

#include "CameraBase.h"

#include "LightSource.h"

#include <unordered_map>
#include <memory>
#include <map>

namespace
{
  enum EShaderType
  {
    eVertexShader,
    eFragmentShader,
    eNumShaders
  };
}

struct Shader::Private
{
  GLuint program;
  GLuint shaders[eNumShaders];
  std::unordered_map<std::string, Attribute> attributes;
  std::unordered_map<std::string, Uniform> uniforms;

  std::map<char const*, GLuint> uniforms2;
};

Shader::Shader(std::string const & p_filename)
{
  d = new Private();
  d->program = glCreateProgram();

  d->shaders[eVertexShader] = createShader(loadFile(p_filename + ".vs"), GL_VERTEX_SHADER);
  d->shaders[eFragmentShader] = createShader(loadFile(p_filename + ".fs"), GL_FRAGMENT_SHADER);

  for (unsigned int index = 0; index < eNumShaders; index++)
  {
    glAttachShader(d->program, d->shaders[index]);
  }

  glLinkProgram(d->program);
  checkProgramError(d->program, GL_LINK_STATUS, std::string("Shader program link error ") + p_filename);

  glValidateProgram(d->program);
  checkProgramError(d->program, GL_LINK_STATUS, std::string("Shader program validation error ") + p_filename);


  /*
  GLint numActiveAttribs = 0;
  GLint numActiveUniforms = 0;
  glGetProgramInterfaceiv(d->program, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numActiveAttribs);
  glGetProgramInterfaceiv(d->program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms);


  // These need to be bound for this to be of any use ... and this needs to happen before linking .. so, um happy times.
  GLint maxAttribNameLength = 0;
  glGetProgramiv(d->program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLength);
  std::vector<GLchar> nameData(maxAttribNameLength);
  for(int attrib = 0; attrib < numActiveAttribs; ++attrib)
  {
    GLint arraySize = 0;
    GLenum type = 0;
    GLsizei actualLength = 0;
    glGetActiveAttrib(d->program, attrib, GLsizei(nameData.size()), &actualLength, &arraySize, &type, &nameData[0]);
    std::string name((char*) &nameData[0], actualLength);
    Attribute attribute;
    attribute.name = name;
    attribute.id = attrib;
    attribute.size = arraySize;
    attribute.type = type;
    d->attributes[name] = attribute;
  }

  for(int unif = 0; unif < numActiveUniforms; ++unif)
  {
    GLint arraySize = 0;
    GLenum type = 0;
    GLsizei actualLength = 0;
    glGetActiveUniform(d->program, unif, GLsizei(nameData.size()), &actualLength, &arraySize, &type, &nameData[0]);
    std::string name((char*) &nameData[0], actualLength);
    Uniform uniform;
    uniform.name = name;
    uniform.id = unif;
    uniform.size = arraySize;
    uniform.type = type;
    d->uniforms[name] = uniform;
  }
  */
}

bool Shader::getAttribute(std::string const & p_name, Shader::Attribute & p_valueOut) const
{
  auto iter = d->attributes.find(p_name);
  if(iter != d->attributes.end())
  {
    p_valueOut = iter->second;
    return true;
  }
  else
  {
    return false;
  }
}

GLuint Shader::getUniformLocation(const char * p_name)
{

  auto iter = d->uniforms2.find(p_name);
  if(iter == d->uniforms2.end())
  {
    GLuint location = glGetUniformLocation(d->program, p_name);
    if(location == 0xffffffff)
    {
      fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", p_name);
    }
    d->uniforms2[p_name] = location;
    return location;
  }
  return iter->second;
}


bool Shader::getUniform(std::string const & p_name, Shader::Uniform & p_valueOut) const
{
  auto iter = d->uniforms.find(p_name);
  if(iter != d->uniforms.end())
  {
    p_valueOut = iter->second;
    return true;
  }
  else
  {
    return false;
  }
}

Shader::~Shader()
{
  for (unsigned int index = 0; index < eNumShaders; index++)
  {
    glDetachShader(d->program, d->shaders[index]);
    glDeleteShader(d->shaders[index]);
  }

  glDeleteProgram(d->program);
  delete d;
}

void Shader::bind()
{
  glUseProgram(d->program);
}

void Shader::unbind()
{
  glUseProgram(0);
}


std::string Shader::loadFile(std::string const & p_filename)
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

GLuint Shader::createShader(std::string const & p_source, GLenum p_shaderType)
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


void Shader::checkShaderError(GLuint p_shader, GLuint p_flag, const std::string & p_errorMessage)
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

void Shader::checkProgramError(GLuint p_program, GLuint p_flag, const std::string & p_errorMessage)
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
