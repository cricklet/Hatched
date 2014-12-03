#include "camera.h"

#include <iostream>

Camera::Camera() {
  this->location = glm::vec3(3.0f, 1.0f, 1.0f);
  this->origin = glm::vec3(0,0,0);
  this->up = glm::vec3(0,0,1);

  this->projTrans = glm::perspective(
    45.0f, // fov y
    800.0f / 600.0f, // aspect
    0.2f,  // near
    10.0f  //far
  );

  this->Update();
}

Camera::~Camera() {
}

void
Camera::SetupTransforms(GLint viewTransUniform, GLint projTransUniform) {
  glUniformMatrix4fv(viewTransUniform, 1, GL_FALSE, glm::value_ptr(this->viewTrans));
  glUniformMatrix4fv(projTransUniform, 1, GL_FALSE, glm::value_ptr(this->projTrans));
}

void
Camera::HandleEvent(SDL_Event event) {
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
  this->Update();
}

void
Camera::Update() {
  this->viewTrans = glm::lookAt(
    this->location, // location of camera
    this->origin, // look at
    this->up  // camera up vector
  );
}
