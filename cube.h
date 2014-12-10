#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sys/time.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include "helper.h"

class Cube {
public:
  Cube(const char *texture);
  ~Cube();
  void Render(const Uniforms &uniforms);
  void BindToShader(GLuint shaderProgram);

private:
  GLuint vao;
  GLuint vbo;

  glm::vec3 color;
  GLint textureIndex;
};
