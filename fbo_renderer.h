#define _USE_MATH_DEFINES
#include <cmath>

#define GLEW_STATIC
#include <GL/glew.h>

#include <string>
#include <vector>
using namespace std;

#include "helper.h"

#ifndef FBO_RENDERER_H_
#define FBO_RENDERER_H_

static GLfloat vertices[] = {
    -1, -1, 0, 0,
    -1, 1, 0, 1,
    1, 1, 1, 1,

    1, 1, 1, 1,
    1, -1, 1, 0,
    -1, -1, 0, 0,
};

static GLuint vertexStride = sizeof(GLfloat) * 4;
static void *positionOffset = 0;
static void *coordOffset = (void *) (2 * sizeof(GLfloat));
static int numElements = 6;

class FBORenderer {
public:
  FBORenderer() {
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);
    checkErrors();

    glGenBuffers(1, &this->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    checkErrors();

  }
  ~FBORenderer() {
    glDeleteVertexArrays(1, &this->vao);
    glDeleteBuffers(1, &this->vbo);
  }

  void BindToShader(GLuint shaderProgram) {
    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

    GLint posAttrib = glGetAttribLocation(shaderProgram, "inVertPosition");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, vertexStride, positionOffset);
    glEnableVertexAttribArray(posAttrib);
    checkErrors();

    GLint coordAttrib = glGetAttribLocation(shaderProgram, "inVertBufferCoord");
    glVertexAttribPointer(coordAttrib, 2, GL_FLOAT, GL_FALSE, vertexStride, coordOffset);
    glEnableVertexAttribArray(coordAttrib);
    checkErrors();
  }

  void Render() const {
    glBindVertexArray(this->vao);

    // Disable tests
    glDisable(GL_DEPTH_TEST);

    // Render the fbo quad
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glDrawArrays(GL_TRIANGLES, 0, numElements);
  }

private:
  GLuint vbo, vao;
};

#endif
