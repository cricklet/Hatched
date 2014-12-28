#ifndef TEXTURES_H
#define TEXTURES_H

#include <iostream>
#include <stdexcept>

#include <string>
#include <map>
using namespace std;

#define GLEW_STATIC
#include <GL/glew.h>

class Texture {
public:
  Texture(string filename = "");
  ~Texture();
  int index;
  GLuint texture;

private: // disable copying
  Texture(const Texture& that) = delete;
  Texture& operator = (const Texture &that) = delete;
};

#endif
