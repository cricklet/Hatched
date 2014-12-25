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
  virtual ~Camera() {};
  virtual void SetupTransforms(GLint viewTransUniform, GLint projTransUniform) = 0;
  virtual void HandleEvent(SDL_Event event) = 0;
  virtual void Think(float dt) = 0;
};

class RotationCamera : public Camera {
 public:
  RotationCamera();

  void SetupTransforms(GLint viewTransUniform, GLint projTransUniform);
  void HandleEvent(SDL_Event event);
  void Think(float dt);

 private:
  glm::vec3 location;
  glm::vec3 origin;
  glm::vec3 up;

  glm::mat4 viewTrans;
  glm::mat4 projTrans;
};

class FPSCamera : public Camera {
 public:
  FPSCamera();

  void SetupTransforms(GLint viewTransUniform, GLint projTransUniform);
  void HandleEvent(SDL_Event event);
  void Think(float dt);

 private:

  glm::vec3 location;
  float pitch;
  float yaw;

  glm::mat4 viewTrans;
  glm::mat4 projTrans;

  bool w = false, a = false, s = false, d = false;
};
