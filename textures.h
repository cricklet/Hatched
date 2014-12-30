#ifndef TEXTURES_H
#define TEXTURES_H

#include <iostream>
#include <stdexcept>
#include <memory>

#include <string>
#include <map>
using namespace std;

#define GLEW_STATIC
#include <GL/glew.h>

class Texture {
public:
  Texture();
  void InitializeAs2D(string filename = "");
  void InitializeAsCubemap();
  ~Texture();

  bool initialized = false;
  int index;
  GLuint texture;

private: // disable copying
  Texture(const Texture& that) = delete;
  Texture& operator = (const Texture &that) = delete;
};

shared_ptr<Texture> makeTextureAs2D(string filename = "");

#endif
