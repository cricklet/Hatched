#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sys/time.h>

#define GLEW_STATIC
#include <GL/glew.h>

class Cube {
 public:
  Cube();
  ~Cube();
  void Render(float time, GLint modelTransUniform);
  void BindToShader(GLuint shaderProgram);

 private:
  GLuint vao;
  GLuint vbo;
  
  GLint shaderProgram;
};
