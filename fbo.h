#define _USE_MATH_DEFINES
#include <cmath>

#define GLEW_STATIC
#include <GL/glew.h>

#include <string>
#include <vector>
using namespace std;

#include "helper.h"
#include "textures.h"

#ifndef FBO_H_
#define FBO_H_

class FBO {
 public:
  FBO(int width, int height);
  ~FBO();
  void BindToShader(GLuint shaderProgram);
  GLuint GetFrameBuffer();
  Texture GetAttachment0();
  Texture GetAttachment1();
  Texture GetDepth();
  void Render();

 private:
  GLuint fbo, vbo, vao;
  Texture attachment0;
  Texture attachment1;
  Texture depth;
};

FBO *FBOFactory(string id, int width, int height);

#endif
