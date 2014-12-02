#include "camera.h"

Camera::Camera() {
  this->location = glm::vec3(3.0f, 1.0f, 1.0f);
  this->lookAt = glm::vec3(0,0,0);
  this->up = glm::vec3(0,0,1);

  this->projTrans = glm::perspective(
    45.0f, // fov y
    800.0f / 600.0f, // aspect
    0.2f,  // near
    10.0f  //far
  );

  this->Update();
}

Camera::Update() {
  this->viewTrans = glm::lookAt(
    glm::vec3(3.0f, 1.0f, 1.0f), // location of camera
    glm::vec3(0,0,0), // look at
    glm::vec3(0,0,1)  // camera up vector
  );
}
