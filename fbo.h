#define _USE_MATH_DEFINES
#include <cmath>

#define GLEW_STATIC
#include <GL/glew.h>

#include <string>
#include <vector>
using namespace std;

#include "helper.h"

class FBO {
 public:
  FBO(int width, int height, GLuint pixelType = GL_RGB);
  void BindToShader(GLuint shaderProgram);
  GLuint GetFrameBuffer();
  int GetTextureIndex();
  void Render();

 private:
  GLuint fbo, vbo, vao;
  int textureIndex;
};
