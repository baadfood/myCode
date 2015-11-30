#ifndef Texture_h_
#define Texture_h_

#include <string>

class Texture
{
public:
  Texture(std::string const & p_filename);
  virtual ~Texture();

  void bind(unsigned int unit);
  void unbind();

private:
  struct Private;
  Private *d;
  
  Texture(Texture const & p_other) {}
  Texture const & operator=(Texture const & p_other) {}
};

#endif