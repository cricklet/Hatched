#define _USE_MATH_DEFINES
#include <cmath>

#define GLEW_STATIC
#include <GL/glew.h>

#include <string>
#include <vector>
using namespace std;

#include "helper.h"

#ifndef FBO_H_
#define FBO_H_

class FBO {
 public:
  FBO(int width, int height);
  void BindToShader(GLuint shaderProgram);
  GLuint GetFrameBuffer();
  int GetScreenTextureIndex();
  int GetDepthTextureIndex();
  void Render();

 private:
  GLuint fbo, vbo, vao;
  int screenTextureIndex;
  int depthTextureIndex;
};

#endif
