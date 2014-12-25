#ifndef TEXTURES_H
#define TEXTURES_H

#include <iostream>
#include <stdexcept>

#include <string>
#include <map>
using namespace std;

#define GLEW_STATIC
#include <GL/glew.h>

struct Texture {
  int index;
  GLuint texture;
};

Texture newTexture(string filename = "");

#endif
