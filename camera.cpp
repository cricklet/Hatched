#include "camera.h"

#include <iostream>
using namespace std;

RotationCamera::RotationCamera() {
  this->location = glm::vec3(1.2,0,0);
  this->origin = glm::vec3(0,0,0);
  this->up = glm::vec3(0,0,1);

  this->projTrans = glm::perspective(
      45.0f, // fov y
      800.0f / 600.0f, // aspect
      0.1f,  // near
      100.0f  //far
  );
}

RotationCamera::~RotationCamera() {
}

void
RotationCamera::SetupTransforms(GLint viewTransUniform, GLint projTransUniform) {
  glUniformMatrix4fv(viewTransUniform, 1, GL_FALSE, glm::value_ptr(this->viewTrans));
  glUniformMatrix4fv(projTransUniform, 1, GL_FALSE, glm::value_ptr(this->projTrans));
}

void
RotationCamera::HandleEvent(SDL_Event event) {
  if (event.type != SDL_MOUSEMOTION) return;
  if (event.button.button != SDL_BUTTON_LEFT) return;

  float dx = event.motion.xrel;
  float dy = event.motion.yrel;

  // Determine axis & angle to rotate camera
  float vx = dx / (float) 800;
  float vy = - dy / (float) 600;
  float theta = 4.0 * (fabs(vx) + fabs(vy));
  glm::vec3 towards = this->origin - this->location;
  glm::vec3 right = glm::cross(towards, this->up);
  glm::vec3 vector = (right * vx) + (this->up * vy);
  glm::vec3 axis = glm::cross(towards, vector);
  axis = glm::normalize(axis);

  this->location = glm::vec3(glm::rotate(theta, axis) * glm::vec4(this->location, 1.0));
}

void
RotationCamera::Think(float dt) {
  this->viewTrans = glm::lookAt(
      this->location, // location of camera
      this->origin, // look at
      this->up  // camera up vector
  );
}


FPSCamera::FPSCamera() {
  this->location = glm::vec3(1.2,0,0);
  this->pitch = - M_PI / 2;
  this->yaw = 0;

  this->projTrans = glm::perspective(
      45.0f, // fov y
      800.0f / 600.0f, // aspect
      0.1f,  // near
      100.0f  //far
  );
}

FPSCamera::~FPSCamera() {
}

void
FPSCamera::SetupTransforms(GLint viewTransUniform, GLint projTransUniform) {
  glUniformMatrix4fv(viewTransUniform, 1, GL_FALSE, glm::value_ptr(this->viewTrans));
  glUniformMatrix4fv(projTransUniform, 1, GL_FALSE, glm::value_ptr(this->projTrans));
}

static void handleMouse(SDL_Event event, float &pitch, float &yaw) {
  if (event.type != SDL_MOUSEMOTION) return;
  if (event.button.button != SDL_BUTTON_LEFT) return;

  float dx = event.motion.xrel;
  float dy = event.motion.yrel;

  pitch += dy / 400.0f;
  yaw   += dx / 400.0f;
}

static glm::vec3 computeForward(float pitch, float yaw) {
  return glm::vec3(
      sin(pitch) * cos (yaw),
      sin(pitch) * sin(yaw),
      cos(pitch)
  );
}

static glm::vec3 computeRight(glm::vec3 forward, glm::vec3 up) {
  return glm::cross(forward, up);
}

static void handleKeys(SDL_Event event, bool &w, bool &a, bool &s, bool &d) {
  if (event.type == SDL_KEYDOWN) {
    switch (event.key.keysym.sym) {
      case SDLK_w: w = true; break;
      case SDLK_a: a = true; break;
      case SDLK_s: s = true; break;
      case SDLK_d: d = true; break;
    }
  }

  if (event.type == SDL_KEYUP) {
    switch (event.key.keysym.sym) {
      case SDLK_w: w = false; break;
      case SDLK_a: a = false; break;
      case SDLK_s: s = false; break;
      case SDLK_d: d = false; break;
    }
  }
}

void
FPSCamera::HandleEvent(SDL_Event event) {
  handleMouse(event, this->pitch, this->yaw);
  handleKeys(event, this->w, this->a, this->s, this->d);
}

void FPSCamera::Think(float dt) {
  glm::vec3 forward = computeForward(this->pitch, this->yaw);
  glm::vec3 right = computeRight(forward, glm::vec3(0,0,1));

  glm::vec2 dir(0,0);
  if (w) dir.y += 1;
  if (a) dir.x -= 1;
  if (s) dir.y -= 1;
  if (d) dir.x += 1;

  this->location += dt * forward * dir.y;
  this->location += dt * right * dir.x;

  this->viewTrans = glm::lookAt(
      this->location, // location of camera
      this->location + forward, // look at
      glm::vec3(0,0,1)  // camera up vector
  );
}


