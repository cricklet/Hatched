#ifndef HELPER_H
#define HELPER_H

#include <iostream>
#include <stdexcept>

#define GLEW_STATIC
#include <GL/glew.h>

float random(float max);

bool _checkErrors(const char *filename, int line);
#define checkErrors() _checkErrors(__FILE__, __LINE__)

GLuint generateShaderProgram(const char *vertSource, const char *fragSource);

GLuint loadTexture(const char *filename, int index);

int nextTextureIndex();
#endif
