#include "pyramid.h"
#include "helper.h"

static GLfloat vertices[] = {
  -0.5f, -0.5f, -0.5f, 0.5, 0.5, 1, // xyz rgb
  0.5f,  -0.5f, -0.5f, 0.5, 0.5, 1,
  0.5f,   0.5f, -0.5f, 0.5, 0.5, 1,
  -0.5f,  0.5f, -0.5f, 0.5, 0.5, 1,
  0,      0,     0.5f, 0.5, 0.5, 1,
};

static GLint elements[] = {
  0,2,1, 0,3,2,
  0,1,4,
  1,2,4,
  2,3,4,
  3,0,4,
};

static GLuint vertexStride = sizeof(GLfloat) * 6;
static void *positionOffset = (void *) 0;
static void *colorOffset = (void *) (sizeof(GLfloat) * 3);
static int numElements = 6 * 3;

void
Pyramid::BindToShader(GLuint shaderProgram) {
  glBindVertexArray(this->vao);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

  GLint posAttrib = glGetAttribLocation(shaderProgram, "inVertPosition");
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, vertexStride, positionOffset);
  glEnableVertexAttribArray(posAttrib);
  checkErrors();

  GLint colorAttrib = glGetAttribLocation(shaderProgram, "inVertColor");
  glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, vertexStride, colorOffset);
  glEnableVertexAttribArray(colorAttrib);
  checkErrors();
}

Pyramid::Pyramid() {
  // Generate vertex array object
  glGenVertexArrays(1, &this->vao);
  glBindVertexArray(this->vao);
  checkErrors();

  // Store the vertices
  glGenBuffers(1, &this->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  checkErrors();

  // Load the indices of vertices to render
  glGenBuffers(1, &this->ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

  glBindVertexArray(0);
}

Pyramid::~Pyramid() {
  glDeleteBuffers(1, &this->vbo);
  glDeleteVertexArrays(1, &this->vao);
}

void
Pyramid::Render(float time, GLint modelTransUniform) {
  glBindVertexArray(this->vao);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
  checkErrors();

  glEnable(GL_DEPTH_TEST);

  glm::mat4 modelTrans;
  modelTrans = glm::translate(modelTrans, glm::vec3(0,1,0));
  modelTrans = glm::rotate(modelTrans, time, glm::vec3(0,0,1));
  glUniformMatrix4fv(modelTransUniform, 1, GL_FALSE, glm::value_ptr(modelTrans));

  glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_INT, 0);
  checkErrors();
}
