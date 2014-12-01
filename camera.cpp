#include "helper.h"
#include "camera.h"

Camera::Camera() {
  this->viewTrans = glm::lookAt(
    glm::vec3(3.0f, 1.0f, 1.0f), // location of camera
    glm::vec3(0,0,0), // direction of camera
    glm::vec3(0,0,1)  // camera up vector
  );

  this->projTrans = glm::perspective(
    45.0f, // fov y
    800.0f / 600.0f, // aspect
    0.2f,  // near
    10.0f  //far
  );
}

void Camera::BindToShader(GLuint shaderProgram) {
  GLint viewTransUniform  = glGetUniformLocation(shaderProgram, "inVertViewTrans");
  GLint projTransUniform  = glGetUniformLocation(shaderProgram, "inVertProjTrans");
}


