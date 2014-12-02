#include "cube.h"
#include "helper.h"

GLfloat vertices[] = {
  -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // location, normal
  0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
  0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
  0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
  -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

  -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
  0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 
  0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
  0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
  -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

  -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
  -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
  -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
  -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
  -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
  -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
  0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
  0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
  0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
  0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

  -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
  0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
  0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
  0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

  -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
  0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
  -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
  -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
};

static GLuint vertexStride  = sizeof(GLfloat) * 6;
static void *positionOffset = (void *) 0;
static void *normalOffset = (void *) (sizeof(GLfloat) * 3);
static int numElements = 36;

void
Cube::BindToShader(GLuint shaderProgram) {
  glBindVertexArray(this->vao);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

  GLint posAttrib = glGetAttribLocation(shaderProgram, "inVertPosition");
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, vertexStride, positionOffset);
  glEnableVertexAttribArray(posAttrib);
  checkErrors();

  GLint normAttrib = glGetAttribLocation(shaderProgram, "inVertNorm");
  if (normAttrib != -1) {
    glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, vertexStride, normalOffset);
    glEnableVertexAttribArray(normAttrib);
    checkErrors();
  }
}

Cube::Cube() {
  // Generate vertex array object
  glGenVertexArrays(1, &this->vao);
  glBindVertexArray(this->vao);
  checkErrors();

  // Store the vertices
  glGenBuffers(1, &this->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  checkErrors();

  // Generate random color
  color = glm::vec3(random(1), random(1), random(1));

  glBindVertexArray(0);
}

Cube::~Cube() {
  glDeleteBuffers(1, &this->vbo);
  glDeleteVertexArrays(1, &this->vao);
}

void
Cube::Render(float time,
	     GLint modelTransUniform, const glm::mat4 worldTrans,
	     GLint colorUniform) {
  glBindVertexArray(this->vao);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

  glEnable(GL_DEPTH_TEST);

  glm::mat4 modelTrans;
  modelTrans = glm::rotate(modelTrans, time, glm::vec3(0,0,1));
  glUniformMatrix4fv(modelTransUniform, 1, GL_FALSE, glm::value_ptr(worldTrans * modelTrans));

  glUniform3fv(colorUniform, 1, glm::value_ptr(this->color));

  glDrawArrays(GL_TRIANGLES, 0, numElements);
}
