#include "cube.h"
#include "helper.h"

GLfloat vertices[] = {
  -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0,0, // location, normal, uv
  0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1,0,
  0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1,1,
  0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1,1,
  -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0,1,
  -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0,0,

  -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0,0,
  0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1,0,
  0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1,1,
  0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1,1,
  -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0,1,
  -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0,0,

  -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0,0,
  -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1,0,
  -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1,1,
  -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1,1,
  -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0,1,
  -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0,0,

  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0,0,
  0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1,0,
  0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1,1,
  0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1,1,
  0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0,1,
  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0,0,

  -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0,0,
  0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1,0,
  0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1,1,
  0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1,1,
  -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0,1,
  -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0,0,

  -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0,0,
  0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1,0,
  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1,1,
  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1,1,
  -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0,1,
  -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0,0,
};

static GLuint vertexStride  = sizeof(GLfloat) * 8;
static void *positionOffset = (void *) 0;
static void *normalOffset = (void *) (sizeof(GLfloat) * 3);
static void *uvOffset = (void *) (sizeof(GLfloat) * 6);
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

  GLint uvAttrib = glGetAttribLocation(shaderProgram, "inVertUV");
  if (uvAttrib != -1) {
    glVertexAttribPointer(uvAttrib, 2, GL_FLOAT, GL_FALSE, vertexStride, uvOffset);
    glEnableVertexAttribArray(uvAttrib);
    checkErrors();
  }
}

Cube::Cube(const char *textureSource) {
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
  this->color = glm::vec3(random(1), random(1), random(1));

  // Bind texture
  this->textureIndex = nextTextureIndex();
  loadTexture(textureSource, this->textureIndex);
  checkErrors();
}

Cube::~Cube() {
  glDeleteBuffers(1, &this->vbo);
  glDeleteVertexArrays(1, &this->vao);
}

void
Cube::Render(const Uniforms &uniforms) {
  glBindVertexArray(this->vao);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
  checkErrors();

  glEnable(GL_DEPTH_TEST);
  checkErrors();

  glUniform3fv(uniforms.color, 1, glm::value_ptr(this->color));
  glUniform1i(uniforms.texture, this->textureIndex);
  checkErrors();

  glDrawArrays(GL_TRIANGLES, 0, numElements);
  checkErrors();
}
