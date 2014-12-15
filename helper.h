#ifndef HELPER_H
#define HELPER_H

#include <iostream>
#include <stdexcept>

#include <string>
using namespace std;

#define GLEW_STATIC
#include <GL/glew.h>

struct Uniforms {
  GLint modelTrans;
  GLint viewTrans;
  GLint projTrans;
  GLint useTexture;
  GLint color;
  GLint texture;
  GLint numTiles;
  GLint tilesTexture;
  GLint lightDir;
  GLint buffer;
  GLint random;
};

float random(float max);

bool _checkErrors(const char *filename, int line);
#define checkErrors() _checkErrors(__FILE__, __LINE__)

GLuint generateShaderProgram(string vertSource, string fragSource);

GLuint loadTexture(string filename, int index);

int nextTextureIndex();

Uniforms getUniforms(GLint shaderProgram);
#endif
