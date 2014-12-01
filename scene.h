#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sys/time.h>

#define GLEW_STATIC
#include <GL/glew.h>

class Scene {
public:
  Scene();
  ~Scene();
  void Render(GLint viewTransUniform, GLint projTransUniform, float t);

private:
  GLuint vao;
  GLuint vbo;
  GLuint shaderProgram;
};
