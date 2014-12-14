#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#define GLEW_STATIC
#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

class Camera {
 public:
  Camera();
  ~Camera();

  void SetupTransforms(GLint viewTransUniform, GLint projTransUniform);
  void HandleEvent(SDL_Event event);

 private:
  void Update();

  glm::vec3 location;
  glm::vec3 origin;
  glm::vec3 up;

  glm::mat4 viewTrans;
  glm::mat4 projTrans;
};
