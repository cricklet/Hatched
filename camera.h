#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#define GLEW_STATIC
#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "uniforms.h"

class Camera {
public:
  virtual ~Camera() {};
  virtual void SetupTransforms(const Uniforms &u);
  virtual void HandleEvent(SDL_Event event) = 0;
  virtual void Think(float dt) = 0;

protected:
  glm::mat4 viewTrans;
  glm::mat4 projTrans;
};

class RotationCamera : public Camera {
 public:
  RotationCamera();

  void HandleEvent(SDL_Event event);
  void Think(float dt);

 private:
  glm::vec3 location;
  glm::vec3 origin;
  glm::vec3 up;
};

class FPSCamera : public Camera {
 public:
  FPSCamera();

  void HandleEvent(SDL_Event event);
  void Think(float dt);

 private:

  glm::vec3 location;
  float pitch;
  float yaw;

  bool w = false, a = false, s = false, d = false;
};
