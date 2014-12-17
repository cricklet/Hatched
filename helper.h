#ifndef HELPER_H
#define HELPER_H

#include <iostream>
#include <stdexcept>

#include <string>
using namespace std;

#define GLEW_STATIC
#include <GL/glew.h>

const int WIDTH = 800;
const int HEIGHT = 600;

float random(float max);

bool _checkErrors(const char *filename, int line);
#define checkErrors() _checkErrors(__FILE__, __LINE__)

GLuint generateShaderProgram(string vertSource, string fragSource);

#endif
