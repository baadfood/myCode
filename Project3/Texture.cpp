#include "Texture.h"

#include <GL/glew.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cassert>
#include <iostream>

struct Texture::Private 
{
  GLuint textureHandle;
};

Texture::Texture(std::string const & p_filename)
{
  d = new Private;

  int width;
  int height;
  int numComponents;

  unsigned char * imageData = stbi_load(p_filename.c_str(), &width, &height, &numComponents, 4);

  if (imageData == NULL)
  {
    std::cerr << "Image data null for image file " << p_filename << std::endl;
  }

  glGenTextures(1, &d->textureHandle);
  glBindTexture(GL_TEXTURE_2D, d->textureHandle);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
  
  GLenum errorCode = glGetError();
  if(errorCode)
  {
    std::cout << "gl error while loading textures\n";
  }

  stbi_image_free(imageData);
}

Texture::~Texture()
{
  glDeleteTextures(1, &d->textureHandle);
  delete d;
}

void Texture::bind(unsigned int p_unit)
{
  assert(p_unit < 32);

  glActiveTexture(GL_TEXTURE0 + p_unit);
  glBindTexture(GL_TEXTURE_2D, d->textureHandle);
  GLenum errorCode = glGetError();
  if(errorCode)
  {
    std::cout << "gl error while binding texture\n";
  }
}

void Texture::unbind()
{
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);
}
