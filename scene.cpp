#include "scene.h"

static GLfloat vertices[] = {
  -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // x y z r g b
  0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
  0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
  -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
};

static GLuint vertexStride   = sizeof(GLfloat) * 6;
static void * locationOffset = (void *) 0;
static void * colorOffset    = (void *) (sizeof(GLfloat) * 3);
